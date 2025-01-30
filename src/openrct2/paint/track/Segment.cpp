/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Segment.h"

#include "../../ride/TrackData.h"
#include "../../world/tile_element/TrackElement.h"
#include "../tile_element/Paint.TileElement.h"

namespace OpenRCT2::BlockedSegments
{
    // clang-format off
    static constexpr const std::array<bool, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> kBlockedSegmentsShouldInvertPrePostCall[] = {
        {{ false }}, // Flat
        {{ false }}, // EndStation
        {{ false }}, // BeginStation
        {{ false }}, // MiddleStation
        {{ false }}, // Up25
        {{ false }}, // Up60
        {{ false }}, // FlatToUp25
        {{ false }}, // Up25ToUp60
        {{ false }}, // Up60ToUp25
        {{ false }}, // Up25ToFlat
        {{ false }}, // Down25
        {{ false }}, // Down60
        {{ false }}, // FlatToDown25
        {{ false }}, // Down25ToDown60
        {{ false }}, // Down60ToDown25
        {{ false }}, // Down25ToFlat
        {{ false, false, false, false, false, false, false }}, // LeftQuarterTurn5Tiles
        {{ false, false, false, false, false, false, false }}, // RightQuarterTurn5Tiles
        {{ false }}, // FlatToLeftBank
        {{ false }}, // FlatToRightBank
        {{ false }}, // LeftBankToFlat
        {{ false }}, // RightBankToFlat
        {{ false, false, false, false, false, false, false }}, // BankedLeftQuarterTurn5Tiles
        {{ false, false, false, false, false, false, false }}, // BankedRightQuarterTurn5Tiles
        {{ false }}, // LeftBankToUp25
        {{ false }}, // RightBankToUp25
        {{ false }}, // Up25ToLeftBank
        {{ false }}, // Up25ToRightBank
        {{ false }}, // LeftBankToDown25
        {{ false }}, // RightBankToDown25
        {{ false }}, // Down25ToLeftBank
        {{ false }}, // Down25ToRightBank
        {{ false }}, // LeftBank
        {{ false }}, // RightBank
        {{ false, false, false, false, false, false, false }}, // LeftQuarterTurn5TilesUp25
        {{ false, false, false, false, false, false, false }}, // RightQuarterTurn5TilesUp25
        {{ false, false, false, false, false, false, false }}, // LeftQuarterTurn5TilesDown25
        {{ false, false, false, false, false, false, false }}, // RightQuarterTurn5TilesDown25
        {{ false, false, false, false }}, // SBendLeft
        {{ false, false, false, false }}, // SBendRight
        {{ false, false, false, false, false, false, false, false, false, false }}, // LeftVerticalLoop
        {{ false, false, false, false, false, false, false, false, false, false }}, // RightVerticalLoop
        {{ false, false, false, false }}, // LeftQuarterTurn3Tiles
        {{ false, false, false, false }}, // RightQuarterTurn3Tiles
        {{ false, false, false, false }}, // LeftBankedQuarterTurn3Tiles
        {{ false, false, false, false }}, // RightBankedQuarterTurn3Tiles
        {{ false, false, false, false }}, // LeftQuarterTurn3TilesUp25
        {{ false, false, false, false }}, // RightQuarterTurn3TilesUp25
        {{ false, false, false, false }}, // LeftQuarterTurn3TilesDown25
        {{ false, false, false, false }}, // RightQuarterTurn3TilesDown25
        {{ false }}, // LeftQuarterTurn1Tile
        {{ false }}, // RightQuarterTurn1Tile
        {{ false, false, true }}, // LeftTwistDownToUp
        {{ false, false, true }}, // RightTwistDownToUp
        {{ true, false, false }}, // LeftTwistUpToDown
        {{ true, false, false }}, // RightTwistUpToDown
        {{ false, false, false, false }}, // HalfLoopUp
        {{ false, false, false, false }}, // HalfLoopDown
        {{ false, false, true }}, // LeftCorkscrewUp
        {{ false, false, true }}, // RightCorkscrewUp
        {{ true, false, false }}, // LeftCorkscrewDown
        {{ true, false, false }}, // RightCorkscrewDown
        {{ false }}, // FlatToUp60
        {{ false }}, // Up60ToFlat
        {{ false }}, // FlatToDown60
        {{ false }}, // Down60ToFlat
        {{ false, false, false, false, false, false, false, false, false }}, // TowerBase
        {{ false, false }}, // TowerSection
        {{ false }}, // FlatCovered
        {{ false }}, // Up25Covered
        {{ false }}, // Up60Covered
        {{ false }}, // FlatToUp25Covered
        {{ false }}, // Up25ToUp60Covered
        {{ false }}, // Up60ToUp25Covered
        {{ false }}, // Up25ToFlatCovered
        {{ false }}, // Down25Covered
        {{ false }}, // Down60Covered
        {{ false }}, // FlatToDown25Covered
        {{ false }}, // Down25ToDown60Covered
        {{ false }}, // Down60ToDown25Covered
        {{ false }}, // Down25ToFlatCovered
        {{ false, false, false, false, false, false, false }}, // LeftQuarterTurn5TilesCovered
        {{ false, false, false, false, false, false, false }}, // RightQuarterTurn5TilesCovered
        {{ false, false, false, false }}, // SBendLeftCovered
        {{ false, false, false, false }}, // SBendRightCovered
        {{ false, false, false, false }}, // LeftQuarterTurn3TilesCovered
        {{ false, false, false, false }}, // RightQuarterTurn3TilesCovered
        {{ false, false, false, false, false, false, false, false }}, // LeftHalfBankedHelixUpSmall
        {{ false, false, false, false, false, false, false, false }}, // RightHalfBankedHelixUpSmall
        {{ false, false, false, false, false, false, false, false }}, // LeftHalfBankedHelixDownSmall
        {{ false, false, false, false, false, false, false, false }}, // RightHalfBankedHelixDownSmall
        {{ false, false, false, false, false, false, false, false, false, false, false, false, false, false }}, // LeftHalfBankedHelixUpLarge
        {{ false, false, false, false, false, false, false, false, false, false, false, false, false, false }}, // RightHalfBankedHelixUpLarge
        {{ false, false, false, false, false, false, false, false, false, false, false, false, false, false }}, // LeftHalfBankedHelixDownLarge
        {{ false, false, false, false, false, false, false, false, false, false, false, false, false, false }}, // RightHalfBankedHelixDownLarge
        {{ false }}, // LeftQuarterTurn1TileUp60
        {{ false }}, // RightQuarterTurn1TileUp60
        {{ false }}, // LeftQuarterTurn1TileDown60
        {{ false }}, // RightQuarterTurn1TileDown60
        {{ false }}, // Brakes
        {{ false }}, // Booster
        {{ false }}, // Maze
        {{ false, false, false, false, false, false, false }}, // LeftQuarterBankedHelixLargeUp
        {{ false, false, false, false, false, false, false }}, // RightQuarterBankedHelixLargeUp
        {{ false, false, false, false, false, false, false }}, // LeftQuarterBankedHelixLargeDown
        {{ false, false, false, false, false, false, false }}, // RightQuarterBankedHelixLargeDown
        {{ false, false, false, false, false, false, false }}, // LeftQuarterHelixLargeUp
        {{ false, false, false, false, false, false, false }}, // RightQuarterHelixLargeUp
        {{ false, false, false, false, false, false, false }}, // LeftQuarterHelixLargeDown
        {{ false, false, false, false, false, false, false }}, // RightQuarterHelixLargeDown
        {{ false }}, // Up25LeftBanked
        {{ false }}, // Up25RightBanked
        {{ false }}, // Waterfall
        {{ false }}, // Rapids
        {{ false }}, // OnRidePhoto
        {{ false }}, // Down25LeftBanked
        {{ false }}, // Down25RightBanked
        {{ false, false, false, false, false }}, // Watersplash
        {{ false, false, false, false }}, // FlatToUp60LongBase
        {{ false, false, false, false }}, // Up60ToFlatLongBase
        {{ false }}, // Whirlpool
        {{ false, false, false, false }}, // Down60ToFlatLongBase
        {{ false, false, false, false }}, // FlatToDown60LongBase
        {{ false, false, false, false }}, // CableLiftHill
        {{ false, false, false, false, false, false, false }}, // ReverseFreefallSlope
        {{ false, false }}, // ReverseFreefallVertical
        {{ false, false }}, // Up90
        {{ false, false }}, // Down90
        {{ false, false }}, // Up60ToUp90
        {{ false }}, // Down90ToDown60
        {{ false }}, // Up90ToUp60
        {{ false, false }}, // Down60ToDown90
        {{ false }}, // BrakeForDrop
        {{ false, false, false, false, false }}, // LeftEighthToDiag
        {{ false, false, false, false, false }}, // RightEighthToDiag
        {{ false, false, false, false, false }}, // LeftEighthToOrthogonal
        {{ false, false, false, false, false }}, // RightEighthToOrthogonal
        {{ false, false, false, false, false }}, // LeftEighthBankToDiag
        {{ false, false, false, false, false }}, // RightEighthBankToDiag
        {{ false, false, false, false, false }}, // LeftEighthBankToOrthogonal
        {{ false, false, false, false, false }}, // RightEighthBankToOrthogonal
        {{ false, false, false, false }}, // DiagFlat
        {{ false, false, false, false }}, // DiagUp25
        {{ false, false, false, false }}, // DiagUp60
        {{ false, false, false, false }}, // DiagFlatToUp25
        {{ false, false, false, false }}, // DiagUp25ToUp60
        {{ false, false, false, false }}, // DiagUp60ToUp25
        {{ false, false, false, false }}, // DiagUp25ToFlat
        {{ false, false, false, false }}, // DiagDown25
        {{ false, false, false, false }}, // DiagDown60
        {{ false, false, false, false }}, // DiagFlatToDown25
        {{ false, false, false, false }}, // DiagDown25ToDown60
        {{ false, false, false, false }}, // DiagDown60ToDown25
        {{ false, false, false, false }}, // DiagDown25ToFlat
        {{ false, false, false, false }}, // DiagFlatToUp60
        {{ false, false, false, false }}, // DiagUp60ToFlat
        {{ false, false, false, false }}, // DiagFlatToDown60
        {{ false, false, false, false }}, // DiagDown60ToFlat
        {{ false, false, false, false }}, // DiagFlatToLeftBank
        {{ false, false, false, false }}, // DiagFlatToRightBank
        {{ false, false, false, false }}, // DiagLeftBankToFlat
        {{ false, false, false, false }}, // DiagRightBankToFlat
        {{ false, false, false, false }}, // DiagLeftBankToUp25
        {{ false, false, false, false }}, // DiagRightBankToUp25
        {{ false, false, false, false }}, // DiagUp25ToLeftBank
        {{ false, false, false, false }}, // DiagUp25ToRightBank
        {{ false, false, false, false }}, // DiagLeftBankToDown25
        {{ false, false, false, false }}, // DiagRightBankToDown25
        {{ false, false, false, false }}, // DiagDown25ToLeftBank
        {{ false, false, false, false }}, // DiagDown25ToRightBank
        {{ false, false, false, false }}, // DiagLeftBank
        {{ false, false, false, false }}, // DiagRightBank
        {{ false }}, // LogFlumeReverser
        {{ false }}, // SpinningTunnel
        {{ false, false, true }}, // LeftBarrelRollUpToDown
        {{ false, false, true }}, // RightBarrelRollUpToDown
        {{ true, false, false }}, // LeftBarrelRollDownToUp
        {{ true, false, false }}, // RightBarrelRollDownToUp
        {{ false, false, false, false }}, // LeftBankToLeftQuarterTurn3TilesUp25
        {{ false, false, false, false }}, // RightBankToRightQuarterTurn3TilesUp25
        {{ false, false, false, false }}, // LeftQuarterTurn3TilesDown25ToLeftBank
        {{ false, false, false, false }}, // RightQuarterTurn3TilesDown25ToRightBank
        {{ false }}, // PoweredLift
        {{ false, false, false, false, false, false, true }}, // LeftLargeHalfLoopUp
        {{ false, false, false, false, false, false, true }}, // RightLargeHalfLoopUp
        {{ true, false, false, false, false, false, false }}, // LeftLargeHalfLoopDown
        {{ true, false, false, false, false, false, false }}, // RightLargeHalfLoopDown
        {{ false, false, true }}, // LeftFlyerTwistUp
        {{ false, false, true }}, // RightFlyerTwistUp
        {{ false, false, true }}, // LeftFlyerTwistDown
        {{ false, false, true }}, // RightFlyerTwistDown
        {{ false, false, true, true }}, // FlyerHalfLoopUninvertedUp
        {{ false, false, true, true }}, // FlyerHalfLoopInvertedDown
        {{ false, false, true }}, // LeftFlyerCorkscrewUp
        {{ false, false, true }}, // RightFlyerCorkscrewUp
        {{ false, false, true }}, // LeftFlyerCorkscrewDown
        {{ false, false, true }}, // RightFlyerCorkscrewDown
        {{ false, false, false, false }}, // HeartLineTransferUp
        {{ false, false, false, false }}, // HeartLineTransferDown
        {{ false, false, false, false, false, false }}, // LeftHeartLineRoll
        {{ false, false, false, false, false, false }}, // RightHeartLineRoll
        {{ false, false }}, // MinigolfHoleA
        {{ false, false }}, // MinigolfHoleB
        {{ false, false }}, // MinigolfHoleC
        {{ false, false, false }}, // MinigolfHoleD
        {{ false, false, false }}, // MinigolfHoleE
        {{ false, false, false, false }}, // MultiDimInvertedFlatToDown90QuarterLoop
        {{ false, false, false }}, // Up90ToInvertedFlatQuarterLoop
        {{ false, false, false, false }}, // InvertedFlatToDown90QuarterLoop
        {{ false, false, false, false }}, // LeftCurvedLiftHill
        {{ false, false, false, false }}, // RightCurvedLiftHill
        {{ false, false, false, false, false, false }}, // LeftReverser
        {{ false, false, false, false, false, false }}, // RightReverser
        {{ false, false, false, false }}, // AirThrustTopCap
        {{ false, false }}, // AirThrustVerticalDown
        {{ false, false, false, false, false, false, false }}, // AirThrustVerticalDownToLevel
        {{ false }}, // BlockBrakes
        {{ false, false, false, false }}, // LeftBankedQuarterTurn3TileUp25
        {{ false, false, false, false }}, // RightBankedQuarterTurn3TileUp25
        {{ false, false, false, false }}, // LeftBankedQuarterTurn3TileDown25
        {{ false, false, false, false }}, // RightBankedQuarterTurn3TileDown25
        {{ false, false, false, false, false, false, false }}, // LeftBankedQuarterTurn5TileUp25
        {{ false, false, false, false, false, false, false }}, // RightBankedQuarterTurn5TileUp25
        {{ false, false, false, false, false, false, false }}, // LeftBankedQuarterTurn5TileDown25
        {{ false, false, false, false, false, false, false }}, // RightBankedQuarterTurn5TileDown25
        {{ false }}, // Up25ToLeftBankedUp25
        {{ false }}, // Up25ToRightBankedUp25
        {{ false }}, // LeftBankedUp25ToUp25
        {{ false }}, // RightBankedUp25ToUp25
        {{ false }}, // Down25ToLeftBankedDown25
        {{ false }}, // Down25ToRightBankedDown25
        {{ false }}, // LeftBankedDown25ToDown25
        {{ false }}, // RightBankedDown25ToDown25
        {{ false }}, // LeftBankedFlatToLeftBankedUp25
        {{ false }}, // RightBankedFlatToRightBankedUp25
        {{ false }}, // LeftBankedUp25ToLeftBankedFlat
        {{ false }}, // RightBankedUp25ToRightBankedFlat
        {{ false }}, // LeftBankedFlatToLeftBankedDown25
        {{ false }}, // RightBankedFlatToRightBankedDown25
        {{ false }}, // LeftBankedDown25ToLeftBankedFlat
        {{ false }}, // RightBankedDown25ToRightBankedFlat
        {{ false }}, // FlatToLeftBankedUp25
        {{ false }}, // FlatToRightBankedUp25
        {{ false }}, // LeftBankedUp25ToFlat
        {{ false }}, // RightBankedUp25ToFlat
        {{ false }}, // FlatToLeftBankedDown25
        {{ false }}, // FlatToRightBankedDown25
        {{ false }}, // LeftBankedDown25ToFlat
        {{ false }}, // RightBankedDown25ToFlat
        {{ false, false }}, // LeftQuarterTurn1TileUp90
        {{ false, false }}, // RightQuarterTurn1TileUp90
        {{ false, false }}, // LeftQuarterTurn1TileDown90
        {{ false, false }}, // RightQuarterTurn1TileDown90
        {{ false, false, false }}, // MultiDimUp90ToInvertedFlatQuarterLoop
        {{ false, false, false, false }}, // MultiDimFlatToDown90QuarterLoop
        {{ false, false, false }}, // MultiDimInvertedUp90ToFlatQuarterLoop
        {{ false }}, // RotationControlToggle
        {{ false, false, false, false }}, // FlatTrack1x4A
        {{ false, false, false, false }}, // FlatTrack2x2
        {{ false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false }}, // FlatTrack4x4
        {{ false, false, false, false, false, false, false, false }}, // FlatTrack2x4
        {{ false, false, false, false, false }}, // FlatTrack1x5
        {{ false }}, // FlatTrack1x1A
        {{ false, false, false, false }}, // FlatTrack1x4B
        {{ false }}, // FlatTrack1x1B
        {{ false, false, false, false }}, // FlatTrack1x4C
        {{ false, false, false, false, false, false, false, false, false }}, // FlatTrack3x3
        {{ false, false, false, false, true, true }}, // LeftLargeCorkscrewUp
        {{ false, false, false, false, true, true }}, // RightLargeCorkscrewUp
        {{ true, true, false, false, false, false }}, // LeftLargeCorkscrewDown
        {{ true, true, false, false, false, false }}, // RightLargeCorkscrewDown
        {{ false, false, false, false, false }}, // LeftMediumHalfLoopUp
        {{ false, false, false, false, false }}, // RightMediumHalfLoopUp
        {{ false, false, false, false, false }}, // LeftMediumHalfLoopDown
        {{ false, false, false, false, false }}, // RightMediumHalfLoopDown
        {{ false, false, true }}, // LeftZeroGRollUp
        {{ false, false, true }}, // RightZeroGRollUp
        {{ true, false, false }}, // LeftZeroGRollDown
        {{ true, false, false }}, // RightZeroGRollDown
        {{ false, false, false, true }}, // LeftLargeZeroGRollUp
        {{ false, false, false, true }}, // RightLargeZeroGRollUp
        {{ true, false, false, false }}, // LeftLargeZeroGRollDown
        {{ true, false, false, false }}, // RightLargeZeroGRollDown
        {{ false, false, false, false, false, false, true }}, // LeftFlyerLargeHalfLoopUninvertedUp
        {{ false, false, false, false, false, false, true }}, // RightFlyerLargeHalfLoopUninvertedUp
        {{ false, false, false, true, true, true, true }}, // LeftFlyerLargeHalfLoopInvertedDown
        {{ false, false, false, true, true, true, true }}, // RightFlyerLargeHalfLoopInvertedDown
        {{ false, false, false, false, false, false, true }}, // LeftFlyerLargeHalfLoopInvertedUp
        {{ false, false, false, false, false, false, true }}, // RightFlyerLargeHalfLoopInvertedUp
        {{ false, false, false, false, false, false, true }}, // LeftFlyerLargeHalfLoopUninvertedDown
        {{ false, false, false, false, false, false, true }}, // RightFlyerLargeHalfLoopUninvertedDown
        {{ false, false, false, true }}, // FlyerHalfLoopInvertedUp
        {{ false, false, false, true }}, // FlyerHalfLoopUninvertedDown
        {{ false, false, false, false, false }}, // LeftEighthToDiagUp25
        {{ false, false, false, false, false }}, // RightEighthToDiagUp25
        {{ false, false, false, false, false }}, // LeftEighthToDiagDown25
        {{ false, false, false, false, false }}, // RightEighthToDiagDown25
        {{ false, false, false, false, false }}, // LeftEighthToOrthogonalUp25
        {{ false, false, false, false, false }}, // RightEighthToOrthogonalUp25
        {{ false, false, false, false, false }}, // LeftEighthToOrthogonalDown25
        {{ false, false, false, false, false }}, // RightEighthToOrthogonalDown25
        {{ false, false, false, false }}, // DiagUp25ToLeftBankedUp25
        {{ false, false, false, false }}, // DiagUp25ToRightBankedUp25
        {{ false, false, false, false }}, // DiagLeftBankedUp25ToUp25
        {{ false, false, false, false }}, // DiagRightBankedUp25ToUp25
        {{ false, false, false, false }}, // DiagDown25ToLeftBankedDown25
        {{ false, false, false, false }}, // DiagDown25ToRightBankedDown25
        {{ false, false, false, false }}, // DiagLeftBankedDown25ToDown25
        {{ false, false, false, false }}, // DiagRightBankedDown25ToDown25
        {{ false, false, false, false }}, // DiagLeftBankedFlatToLeftBankedUp25
        {{ false, false, false, false }}, // DiagRightBankedFlatToRightBankedUp25
        {{ false, false, false, false }}, // DiagLeftBankedUp25ToLeftBankedFlat
        {{ false, false, false, false }}, // DiagRightBankedUp25ToRightBankedFlat
        {{ false, false, false, false }}, // DiagLeftBankedFlatToLeftBankedDown25
        {{ false, false, false, false }}, // DiagRightBankedFlatToRightBankedDown25
        {{ false, false, false, false }}, // DiagLeftBankedDown25ToLeftBankedFlat
        {{ false, false, false, false }}, // DiagRightBankedDown25ToRightBankedFlat
        {{ false, false, false, false }}, // DiagFlatToLeftBankedUp25
        {{ false, false, false, false }}, // DiagFlatToRightBankedUp25
        {{ false, false, false, false }}, // DiagLeftBankedUp25ToFlat
        {{ false, false, false, false }}, // DiagRightBankedUp25ToFlat
        {{ false, false, false, false }}, // DiagFlatToLeftBankedDown25
        {{ false, false, false, false }}, // DiagFlatToRightBankedDown25
        {{ false, false, false, false }}, // DiagLeftBankedDown25ToFlat
        {{ false, false, false, false }}, // DiagRightBankedDown25ToFlat
        {{ false, false, false, false }}, // DiagUp25LeftBanked
        {{ false, false, false, false }}, // DiagUp25RightBanked
        {{ false, false, false, false }}, // DiagDown25LeftBanked
        {{ false, false, false, false }}, // DiagDown25RightBanked
        {{ false, false, false, false, false }}, // LeftEighthBankToDiagUp25
        {{ false, false, false, false, false }}, // RightEighthBankToDiagUp25
        {{ false, false, false, false, false }}, // LeftEighthBankToDiagDown25
        {{ false, false, false, false, false }}, // RightEighthBankToDiagDown25
        {{ false, false, false, false, false }}, // LeftEighthBankToOrthogonalUp25
        {{ false, false, false, false, false }}, // RightEighthBankToOrthogonalUp25
        {{ false, false, false, false, false }}, // LeftEighthBankToOrthogonalDown25
        {{ false, false, false, false, false }}, // RightEighthBankToOrthogonalDown25
        {{ false, false, false, false }}, // DiagBrakes
        {{ false, false, false, false }}, // DiagBlockBrakes
        {{ false }}, // Down25Brakes
        {{ false, false, false, false }}, // DiagBooster
        {{ false, false, false, false, false, false, false, false, false, false }}, // DiagFlatToUp60LongBase
        {{ false, false, false, false, false, false, false, false, false, false }}, // DiagUp60ToFlatLongBase
        {{ false, false, false, false, false, false, false, false, false, false }}, // DiagFlatToDown60LongBase
        {{ false, false, false, false, false, false, false, false, false, false }}, // DiagDown60ToFlatLongBase
        {{ false, false, false, false, true, true }}, // LeftEighthDiveLoopUpToOrthogonal
        {{ false, false, false, false, true, true }}, // RightEighthDiveLoopUpToOrthogonal
        {{ true, true, false, false, false, false }}, // LeftEighthDiveLoopDownToDiag
        {{ true, true, false, false, false, false }}, // RightEighthDiveLoopDownToDiag
        {{ false, false, false, false }}, // DiagDown25Brakes
    };
    // clang-format on

    static_assert(std::size(kBlockedSegmentsShouldInvertPrePostCall) == EnumValue(TrackElemType::Count));

    bool GetShouldInvertPrePostCall(const TrackElemType trackElemType, const uint8_t trackSequence)
    {
        return kBlockedSegmentsShouldInvertPrePostCall[EnumValue(trackElemType)][trackSequence];
    }

    // clang-format off
    namespace Narrow
    {
        constexpr uint16_t kFlatSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kEndStationSeq0 = kSegmentsAll;
        constexpr uint16_t kBeginStationSeq0 = kSegmentsAll;
        constexpr uint16_t kMiddleStationSeq0 = kSegmentsAll;
        constexpr uint16_t kUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq4 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn5TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn5TilesSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq4 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn5TilesSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlatToLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq4 = kSegmentsNone;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq4 = kSegmentsNone;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kSBendLeftSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kSBendLeftSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kSBendLeftSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kSBendLeftSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kSBendRightSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kSBendRightSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kSBendRightSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kSBendRightSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftVerticalLoopSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftVerticalLoopSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftVerticalLoopSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftVerticalLoopSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftVerticalLoopSeq4 = kSegmentsNone;
        constexpr uint16_t kLeftVerticalLoopSeq5 = kSegmentsNone;
        constexpr uint16_t kLeftVerticalLoopSeq6 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftVerticalLoopSeq7 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftVerticalLoopSeq8 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftVerticalLoopSeq9 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq4 = kSegmentsNone;
        constexpr uint16_t kRightVerticalLoopSeq5 = kSegmentsNone;
        constexpr uint16_t kRightVerticalLoopSeq6 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightVerticalLoopSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightVerticalLoopSeq8 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightVerticalLoopSeq9 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq1 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq2 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq2 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq2 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq2 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn1TileSeq0 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn1TileSeq0 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftTwistDownToUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftTwistDownToUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftTwistDownToUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightTwistDownToUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightTwistDownToUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightTwistDownToUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftTwistUpToDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftTwistUpToDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftTwistUpToDownSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightTwistUpToDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightTwistUpToDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightTwistUpToDownSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kHalfLoopUpSeq1 = kSegmentsAll;
        constexpr uint16_t kHalfLoopUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kHalfLoopUpSeq3 = kFlatSeq0;
        constexpr uint16_t kHalfLoopDownSeq0 = kFlatSeq0;
        constexpr uint16_t kHalfLoopDownSeq1 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kHalfLoopDownSeq2 = kSegmentsAll;
        constexpr uint16_t kHalfLoopDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftCorkscrewUpSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightCorkscrewUpSeq1 = kSegmentsAll;
        constexpr uint16_t kRightCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftCorkscrewDownSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftCorkscrewDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightCorkscrewDownSeq1 = kSegmentsAll;
        constexpr uint16_t kRightCorkscrewDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlatToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kTowerBaseSeq0 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq1 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq2 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq3 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq4 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq5 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq6 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq7 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq8 = kSegmentsNone;
        constexpr uint16_t kTowerSectionSeq0 = kSegmentsNone;
        constexpr uint16_t kTowerSectionSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatCoveredSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25CoveredSeq0 = kUp25Seq0;
        constexpr uint16_t kUp60CoveredSeq0 = kUp60Seq0;
        constexpr uint16_t kFlatToUp25CoveredSeq0 = kFlatToUp25Seq0;
        constexpr uint16_t kUp25ToUp60CoveredSeq0 = kUp25ToUp60Seq0;
        constexpr uint16_t kUp60ToUp25CoveredSeq0 = kUp60ToUp25Seq0;
        constexpr uint16_t kUp25ToFlatCoveredSeq0 = kUp25ToFlatSeq0;
        constexpr uint16_t kDown25CoveredSeq0 = kDown25Seq0;
        constexpr uint16_t kDown60CoveredSeq0 = kDown60Seq0;
        constexpr uint16_t kFlatToDown25CoveredSeq0 = kFlatToDown25Seq0;
        constexpr uint16_t kDown25ToDown60CoveredSeq0 = kDown25ToDown60Seq0;
        constexpr uint16_t kDown60ToDown25CoveredSeq0 = kDown60ToDown25Seq0;
        constexpr uint16_t kDown25ToFlatCoveredSeq0 = kDown25ToFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kSBendLeftCoveredSeq0 = kSBendLeftSeq0;
        constexpr uint16_t kSBendLeftCoveredSeq1 = kSBendLeftSeq1;
        constexpr uint16_t kSBendLeftCoveredSeq2 = kSBendLeftSeq2;
        constexpr uint16_t kSBendLeftCoveredSeq3 = kSBendLeftSeq3;
        constexpr uint16_t kSBendRightCoveredSeq0 = kSBendRightSeq0;
        constexpr uint16_t kSBendRightCoveredSeq1 = kSBendRightSeq1;
        constexpr uint16_t kSBendRightCoveredSeq2 = kSBendRightSeq2;
        constexpr uint16_t kSBendRightCoveredSeq3 = kSBendRightSeq3;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq0 = kLeftQuarterTurn3TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq1 = kLeftQuarterTurn3TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq2 = kLeftQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq3 = kLeftQuarterTurn3TilesSeq3;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq0 = kRightQuarterTurn3TilesSeq0;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq1 = kRightQuarterTurn3TilesSeq1;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq2 = kRightQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq3 = kRightQuarterTurn3TilesSeq3;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq5 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq7 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq1 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq5 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq5 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq7 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq1 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq5 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq4 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq7 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq8 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq10 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq11 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq12 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq13 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq1 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq4 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq7 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq8 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq10 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq11 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq12 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq13 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq4 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq7 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq8 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq10 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq11 = kSegmentsNone;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq12 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq13 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq1 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq4 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq7 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq8 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq10 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq11 = kSegmentsNone;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq12 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq13 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn1TileUp60Seq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn1TileUp60Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn1TileDown60Seq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn1TileDown60Seq0 = kSegmentsAll;
        constexpr uint16_t kBrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kBoosterSeq0 = kFlatSeq0;
        constexpr uint16_t kMazeSeq0 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq0 = kBankedLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq1 = kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq2 = kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq3 = kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq4 = kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq5 = kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq6 = kBankedLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq0 = kBankedRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq1 = kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq2 = kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq3 = kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq4 = kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq5 = kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq6 = kBankedRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq0 = kBankedLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq1 = kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq2 = kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq3 = kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq4 = kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq5 = kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq6 = kBankedLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq0 = kBankedRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq1 = kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq2 = kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq3 = kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq4 = kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq5 = kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq6 = kBankedRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kUp25LeftBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25RightBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kWaterfallSeq0 = kSegmentsAll;
        constexpr uint16_t kRapidsSeq0 = kFlatSeq0;
        constexpr uint16_t kOnRidePhotoSeq0 = kSegmentsAll;
        constexpr uint16_t kDown25LeftBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25RightBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq0 = kSegmentsAll;
        constexpr uint16_t kWatersplashSeq1 = kSegmentsAll;
        constexpr uint16_t kWatersplashSeq2 = kSegmentsAll;
        constexpr uint16_t kWatersplashSeq3 = kSegmentsAll;
        constexpr uint16_t kWatersplashSeq4 = kSegmentsAll;
        constexpr uint16_t kFlatToUp60LongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kWhirlpoolSeq0 = kSegmentsAll;
        constexpr uint16_t kDown60ToFlatLongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq0 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq1 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq2 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq3 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq0 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq1 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq2 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq3 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq4 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq5 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq6 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallVerticalSeq0 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallVerticalSeq1 = kFlatSeq0;
        constexpr uint16_t kUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kDown90Seq0 = kFlatSeq0;
        constexpr uint16_t kDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kUp60ToUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kDown90ToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp90ToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown90Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kBrakeForDropSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToDiagSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToDiagSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToDiagSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagSeq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthToDiagSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthToOrthogonalSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToOrthogonalSeq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthToOrthogonalSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalSeq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToDiagSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToDiagSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagSeq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthBankToDiagSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthBankToOrthogonalSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalSeq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp60Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp60Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25ToUp60Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToUp60Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25ToUp60Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToUp60Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp60ToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp60ToUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown60Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown60Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25ToDown60Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToDown60Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25ToDown60Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToDown60Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown60ToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown60ToDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToUp60Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToUp60Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp60ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp60ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToDown60Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToDown60Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown60ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown60ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToLeftBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToLeftBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToLeftBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToRightBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToRightBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToRightBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToRightBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankToUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankToUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankToUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankToUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankToUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankToUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25ToLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToLeftBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25ToLeftBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToLeftBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25ToRightBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToRightBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25ToRightBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToRightBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankToDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankToDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankToDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankToDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankToDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankToDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25ToLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToLeftBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25ToLeftBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToLeftBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25ToRightBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToRightBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25ToRightBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToRightBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLogFlumeReverserSeq0 = kFlatSeq0;
        constexpr uint16_t kSpinningTunnelSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBarrelRollUpToDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBarrelRollUpToDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBarrelRollUpToDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBarrelRollUpToDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBarrelRollUpToDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBarrelRollDownToUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBarrelRollDownToUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBarrelRollDownToUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBarrelRollDownToUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBarrelRollDownToUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBarrelRollDownToUpSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = kSegmentsNone;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq2 = kSegmentsNone;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq2 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kPoweredLiftSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeHalfLoopUpSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeHalfLoopUpSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeHalfLoopUpSeq5 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeHalfLoopUpSeq6 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeHalfLoopUpSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeHalfLoopUpSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeHalfLoopUpSeq5 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeHalfLoopUpSeq6 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeHalfLoopDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeHalfLoopDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeHalfLoopDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeHalfLoopDownSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeHalfLoopDownSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeHalfLoopDownSeq5 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq6 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeHalfLoopDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeHalfLoopDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeHalfLoopDownSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeHalfLoopDownSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeHalfLoopDownSeq5 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopDownSeq6 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerTwistUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerTwistUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerTwistUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerTwistUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerTwistUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerTwistUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerTwistDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq1 = kSegmentsAll;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq3 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight, PaintSegment::topRight);
        constexpr uint16_t kRightFlyerCorkscrewUpSeq1 = kSegmentsAll;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleASeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleASeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleBSeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleBSeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleCSeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleCSeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleDSeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleDSeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleDSeq2 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleESeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleESeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleESeq2 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = kFlatSeq0;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = kFlatSeq0;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = kFlatSeq0;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = kSegmentsNone;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq1 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq2 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq1 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq2 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq3 = kSegmentsNone;
        constexpr uint16_t kLeftCurvedLiftHillSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftCurvedLiftHillSeq1 = kSegmentsNone;
        constexpr uint16_t kLeftCurvedLiftHillSeq2 = kSegmentsNone;
        constexpr uint16_t kLeftCurvedLiftHillSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightCurvedLiftHillSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightCurvedLiftHillSeq1 = kSegmentsNone;
        constexpr uint16_t kRightCurvedLiftHillSeq2 = kSegmentsNone;
        constexpr uint16_t kRightCurvedLiftHillSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftReverserSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kBlockBrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq2 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq2 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq2 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq2 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq4 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq4 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq4 = kSegmentsNone;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq1 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq4 = kSegmentsNone;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kUp25ToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedFlatToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedFlatToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToLeftBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToRightBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedFlatToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedFlatToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToLeftBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToRightBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn1TileUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn1TileUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn1TileUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kRightQuarterTurn1TileUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn1TileDown90Seq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn1TileDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn1TileDown90Seq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn1TileDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = kFlatSeq0;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = kFlatSeq0;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = kFlatSeq0;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq0 = kFlatSeq0;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq1 = kFlatSeq0;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq2 = kFlatSeq0;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq3 = kSegmentsNone;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = kFlatSeq0;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = kFlatSeq0;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = kFlatSeq0;
        constexpr uint16_t kRotationControlToggleSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatTrack1x4ASeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq8 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq9 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq10 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq11 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq12 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq13 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq14 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq15 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x1ASeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x1BSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq8 = kSegmentsNone;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeCorkscrewUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeCorkscrewUpSeq3 = kSegmentsNone;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeCorkscrewUpSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewUpSeq3 = kSegmentsNone;
        constexpr uint16_t kRightLargeCorkscrewUpSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewUpSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeCorkscrewDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeCorkscrewDownSeq2 = kSegmentsNone;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeCorkscrewDownSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeCorkscrewDownSeq5 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewDownSeq2 = kSegmentsNone;
        constexpr uint16_t kRightLargeCorkscrewDownSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewDownSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeCorkscrewDownSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftMediumHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftMediumHalfLoopUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftMediumHalfLoopUpSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftMediumHalfLoopUpSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightMediumHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightMediumHalfLoopUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightMediumHalfLoopUpSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightMediumHalfLoopUpSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightMediumHalfLoopUpSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftMediumHalfLoopDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftMediumHalfLoopDownSeq1 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftMediumHalfLoopDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftMediumHalfLoopDownSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftMediumHalfLoopDownSeq4 = kFlatSeq0;
        constexpr uint16_t kRightMediumHalfLoopDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightMediumHalfLoopDownSeq1 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightMediumHalfLoopDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightMediumHalfLoopDownSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightMediumHalfLoopDownSeq4 = kFlatSeq0;
        constexpr uint16_t kLeftZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftZeroGRollUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftZeroGRollUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightZeroGRollUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightZeroGRollUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftZeroGRollDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftZeroGRollDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kRightZeroGRollDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightZeroGRollDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftLargeZeroGRollUpSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightLargeZeroGRollUpSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeZeroGRollDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeZeroGRollDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftLargeZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq3 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeZeroGRollDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightLargeZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq1 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq5 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq6 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq5 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq6 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq5 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq6 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq5 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq6 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq3 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq0 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq1 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq2 = kSegmentsAll;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToDiagUp25Seq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToDiagUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToDiagUp25Seq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagUp25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagUp25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthToDiagUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagUp25Seq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToDiagDown25Seq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToDiagDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToDiagDown25Seq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagDown25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagDown25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthToDiagDown25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagDown25Seq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25LeftBankedSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25LeftBankedSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25LeftBankedSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25LeftBankedSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp25RightBankedSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25RightBankedSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp25RightBankedSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp25RightBankedSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25LeftBankedSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25LeftBankedSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25LeftBankedSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25LeftBankedSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25RightBankedSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25RightBankedSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown25RightBankedSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown25RightBankedSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagUp25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthBankToDiagUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagUp25Seq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagDown25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthBankToDiagDown25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagDown25Seq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBrakesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBrakesSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagBrakesSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBrakesSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagBlockBrakesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBlockBrakesSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagBlockBrakesSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBlockBrakesSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDown25BrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBoosterSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagBoosterSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagBoosterSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq5 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq8 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq5 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq8 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq5 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq8 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq5 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq7 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq8 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq9 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq5 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq1 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq5 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq5 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagDown25BrakesSeq0 = kDiagDown25Seq0;
        constexpr uint16_t kDiagDown25BrakesSeq1 = kDiagDown25Seq1;
        constexpr uint16_t kDiagDown25BrakesSeq2 = kDiagDown25Seq2;
        constexpr uint16_t kDiagDown25BrakesSeq3 = kDiagDown25Seq3;
    } // namespace Narrow

    namespace Inverted
    {
        constexpr uint16_t kFlatSeq0 = Narrow::kFlatSeq0;
        constexpr uint16_t kEndStationSeq0 = Narrow::kEndStationSeq0;
        constexpr uint16_t kBeginStationSeq0 = Narrow::kBeginStationSeq0;
        constexpr uint16_t kMiddleStationSeq0 = Narrow::kMiddleStationSeq0;
        constexpr uint16_t kUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq1 = Narrow::kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq2 = Narrow::kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq3 = Narrow::kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq4 = Narrow::kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq5 = Narrow::kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn5TilesSeq1 = Narrow::kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesSeq2 = Narrow::kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesSeq3 = Narrow::kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesSeq4 = Narrow::kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesSeq5 = Narrow::kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kFlatToLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlatToRightBankSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankToFlatSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankToFlatSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq1 = Narrow::kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq2 = Narrow::kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq3 = Narrow::kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq4 = Narrow::kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq5 = Narrow::kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq1 = Narrow::kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq2 = Narrow::kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq3 = Narrow::kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq4 = Narrow::kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq5 = Narrow::kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankToUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankToUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kUp25ToLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kUp25ToRightBankSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankToDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankToDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kDown25ToLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kDown25ToRightBankSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kSBendLeftSeq0 = Narrow::kSBendLeftSeq0;
        constexpr uint16_t kSBendLeftSeq1 = Narrow::kSBendLeftSeq1;
        constexpr uint16_t kSBendLeftSeq2 = Narrow::kSBendLeftSeq2;
        constexpr uint16_t kSBendLeftSeq3 = Narrow::kSBendLeftSeq3;
        constexpr uint16_t kSBendRightSeq0 = Narrow::kSBendRightSeq0;
        constexpr uint16_t kSBendRightSeq1 = Narrow::kSBendRightSeq1;
        constexpr uint16_t kSBendRightSeq2 = Narrow::kSBendRightSeq2;
        constexpr uint16_t kSBendRightSeq3 = Narrow::kSBendRightSeq3;
        constexpr uint16_t kLeftVerticalLoopSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftVerticalLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftVerticalLoopSeq2 = Narrow::kLeftVerticalLoopSeq2;
        constexpr uint16_t kLeftVerticalLoopSeq3 = Narrow::kLeftVerticalLoopSeq3;
        constexpr uint16_t kLeftVerticalLoopSeq4 = Narrow::kLeftVerticalLoopSeq4;
        constexpr uint16_t kLeftVerticalLoopSeq5 = Narrow::kLeftVerticalLoopSeq5;
        constexpr uint16_t kLeftVerticalLoopSeq6 = Narrow::kLeftVerticalLoopSeq6;
        constexpr uint16_t kLeftVerticalLoopSeq7 = Narrow::kLeftVerticalLoopSeq7;
        constexpr uint16_t kLeftVerticalLoopSeq8 = kSegmentsAll;
        constexpr uint16_t kLeftVerticalLoopSeq9 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq0 = EnumsToFlags(PaintSegment::left, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightVerticalLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kRightVerticalLoopSeq2 = Narrow::kRightVerticalLoopSeq2;
        constexpr uint16_t kRightVerticalLoopSeq3 = Narrow::kRightVerticalLoopSeq3;
        constexpr uint16_t kRightVerticalLoopSeq4 = Narrow::kRightVerticalLoopSeq4;
        constexpr uint16_t kRightVerticalLoopSeq5 = Narrow::kRightVerticalLoopSeq5;
        constexpr uint16_t kRightVerticalLoopSeq6 = Narrow::kRightVerticalLoopSeq6;
        constexpr uint16_t kRightVerticalLoopSeq7 = Narrow::kRightVerticalLoopSeq7;
        constexpr uint16_t kRightVerticalLoopSeq8 = kSegmentsAll;
        constexpr uint16_t kRightVerticalLoopSeq9 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesSeq1 = Narrow::kLeftQuarterTurn3TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq2 = Narrow::kLeftQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesSeq1 = Narrow::kRightQuarterTurn3TilesSeq1;
        constexpr uint16_t kRightQuarterTurn3TilesSeq2 = Narrow::kRightQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq1 = Narrow::kLeftBankedQuarterTurn3TilesSeq1;
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq2 = Narrow::kLeftBankedQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq1 = Narrow::kRightBankedQuarterTurn3TilesSeq1;
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq2 = Narrow::kRightBankedQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq1 = Narrow::kLeftQuarterTurn3TilesUp25Seq1;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq2 = Narrow::kLeftQuarterTurn3TilesUp25Seq2;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq1 = Narrow::kRightQuarterTurn3TilesUp25Seq1;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq2 = Narrow::kRightQuarterTurn3TilesUp25Seq2;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq1 = Narrow::kLeftQuarterTurn3TilesDown25Seq1;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq2 = Narrow::kLeftQuarterTurn3TilesDown25Seq2;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq1 = Narrow::kRightQuarterTurn3TilesDown25Seq1;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq2 = Narrow::kRightQuarterTurn3TilesDown25Seq2;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn1TileSeq0 = Narrow::kLeftQuarterTurn1TileSeq0;
        constexpr uint16_t kRightQuarterTurn1TileSeq0 = Narrow::kRightQuarterTurn1TileSeq0;
        constexpr uint16_t kLeftTwistDownToUpSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftTwistDownToUpSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftTwistDownToUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightTwistDownToUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightTwistDownToUpSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightTwistDownToUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftTwistUpToDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftTwistUpToDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftTwistUpToDownSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightTwistUpToDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightTwistUpToDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightTwistUpToDownSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kHalfLoopUpSeq1 = Narrow::kHalfLoopUpSeq1;
        constexpr uint16_t kHalfLoopUpSeq2 = Narrow::kHalfLoopUpSeq2;
        constexpr uint16_t kHalfLoopUpSeq3 = kFlatSeq0;
        constexpr uint16_t kHalfLoopDownSeq0 = kFlatSeq0;
        constexpr uint16_t kHalfLoopDownSeq1 = Narrow::kHalfLoopDownSeq1;
        constexpr uint16_t kHalfLoopDownSeq2 = Narrow::kHalfLoopDownSeq2;
        constexpr uint16_t kHalfLoopDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftCorkscrewUpSeq1 = Narrow::kLeftCorkscrewUpSeq1;
        constexpr uint16_t kLeftCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightCorkscrewUpSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightCorkscrewUpSeq1 = Narrow::kRightCorkscrewUpSeq1;
        constexpr uint16_t kRightCorkscrewUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftCorkscrewDownSeq1 = Narrow::kLeftCorkscrewDownSeq1;
        constexpr uint16_t kLeftCorkscrewDownSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightCorkscrewDownSeq1 = Narrow::kRightCorkscrewDownSeq1;
        constexpr uint16_t kRightCorkscrewDownSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kFlatToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kTowerBaseSeq0 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq1 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq2 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq3 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq4 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq5 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq6 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq7 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq8 = kSegmentsNone;
        constexpr uint16_t kTowerSectionSeq0 = kSegmentsNone;
        constexpr uint16_t kTowerSectionSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatCoveredSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25CoveredSeq0 = kUp25Seq0;
        constexpr uint16_t kUp60CoveredSeq0 = kUp60Seq0;
        constexpr uint16_t kFlatToUp25CoveredSeq0 = kFlatToUp25Seq0;
        constexpr uint16_t kUp25ToUp60CoveredSeq0 = kUp25ToUp60Seq0;
        constexpr uint16_t kUp60ToUp25CoveredSeq0 = kUp60ToUp25Seq0;
        constexpr uint16_t kUp25ToFlatCoveredSeq0 = kUp25ToFlatSeq0;
        constexpr uint16_t kDown25CoveredSeq0 = kDown25Seq0;
        constexpr uint16_t kDown60CoveredSeq0 = kDown60Seq0;
        constexpr uint16_t kFlatToDown25CoveredSeq0 = kFlatToDown25Seq0;
        constexpr uint16_t kDown25ToDown60CoveredSeq0 = kDown25ToDown60Seq0;
        constexpr uint16_t kDown60ToDown25CoveredSeq0 = kDown60ToDown25Seq0;
        constexpr uint16_t kDown25ToFlatCoveredSeq0 = kDown25ToFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kSBendLeftCoveredSeq0 = kSBendLeftSeq0;
        constexpr uint16_t kSBendLeftCoveredSeq1 = kSBendLeftSeq1;
        constexpr uint16_t kSBendLeftCoveredSeq2 = kSBendLeftSeq2;
        constexpr uint16_t kSBendLeftCoveredSeq3 = kSBendLeftSeq3;
        constexpr uint16_t kSBendRightCoveredSeq0 = kSBendRightSeq0;
        constexpr uint16_t kSBendRightCoveredSeq1 = kSBendRightSeq1;
        constexpr uint16_t kSBendRightCoveredSeq2 = kSBendRightSeq2;
        constexpr uint16_t kSBendRightCoveredSeq3 = kSBendRightSeq3;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq0 = kLeftQuarterTurn3TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq1 = kLeftQuarterTurn3TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq2 = kLeftQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq3 = kLeftQuarterTurn3TilesSeq3;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq0 = kRightQuarterTurn3TilesSeq0;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq1 = kRightQuarterTurn3TilesSeq1;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq2 = kRightQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq3 = kRightQuarterTurn3TilesSeq3;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq10 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq11 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq12 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq13 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq10 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq11 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq12 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq13 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq10 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq11 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq12 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq13 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq10 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq11 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq12 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq13 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftQuarterTurn1TileUp60Seq0 = Narrow::kLeftQuarterTurn1TileUp60Seq0;
        constexpr uint16_t kRightQuarterTurn1TileUp60Seq0 = Narrow::kRightQuarterTurn1TileUp60Seq0;
        constexpr uint16_t kLeftQuarterTurn1TileDown60Seq0 = Narrow::kLeftQuarterTurn1TileDown60Seq0;
        constexpr uint16_t kRightQuarterTurn1TileDown60Seq0 = Narrow::kRightQuarterTurn1TileDown60Seq0;
        constexpr uint16_t kBrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kBoosterSeq0 = kFlatSeq0;
        constexpr uint16_t kMazeSeq0 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq0 = kBankedLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq1 = kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq2 = kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq3 = kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq4 = kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq5 = kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq6 = kBankedLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq0 = kBankedRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq1 = kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq2 = kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq3 = kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq4 = kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq5 = kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq6 = kBankedRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq0 = kBankedLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq1 = kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq2 = kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq3 = kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq4 = kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq5 = kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq6 = kBankedLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq0 = kBankedRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq1 = kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq2 = kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq3 = kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq4 = kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq5 = kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq6 = kBankedRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kUp25LeftBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25RightBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kWaterfallSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRapidsSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kOnRidePhotoSeq0 = Narrow::kOnRidePhotoSeq0;
        constexpr uint16_t kDown25LeftBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25RightBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kWatersplashSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kWatersplashSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kWatersplashSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kWatersplashSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kFlatToUp60LongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kWhirlpoolSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDown60ToFlatLongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq0 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq1 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq2 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq3 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq0 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq1 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq2 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq3 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq4 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq5 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq6 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallVerticalSeq0 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallVerticalSeq1 = kFlatSeq0;
        constexpr uint16_t kUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kDown90Seq0 = kFlatSeq0;
        constexpr uint16_t kDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kUp60ToUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kDown90ToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp90ToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown90Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kBrakeForDropSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagSeq1 = Narrow::kLeftEighthToDiagSeq1;
        constexpr uint16_t kLeftEighthToDiagSeq2 = Narrow::kLeftEighthToDiagSeq2;
        constexpr uint16_t kLeftEighthToDiagSeq3 = Narrow::kLeftEighthToDiagSeq3;
        constexpr uint16_t kLeftEighthToDiagSeq4 = Narrow::kLeftEighthToDiagSeq4;
        constexpr uint16_t kRightEighthToDiagSeq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagSeq1 = Narrow::kRightEighthToDiagSeq1;
        constexpr uint16_t kRightEighthToDiagSeq2 = Narrow::kRightEighthToDiagSeq2;
        constexpr uint16_t kRightEighthToDiagSeq3 = Narrow::kRightEighthToDiagSeq3;
        constexpr uint16_t kRightEighthToDiagSeq4 = Narrow::kRightEighthToDiagSeq4;
        constexpr uint16_t kLeftEighthToOrthogonalSeq0 = Narrow::kLeftEighthToOrthogonalSeq0;
        constexpr uint16_t kLeftEighthToOrthogonalSeq1 = Narrow::kLeftEighthToOrthogonalSeq1;
        constexpr uint16_t kLeftEighthToOrthogonalSeq2 = Narrow::kLeftEighthToOrthogonalSeq2;
        constexpr uint16_t kLeftEighthToOrthogonalSeq3 = Narrow::kLeftEighthToOrthogonalSeq3;
        constexpr uint16_t kLeftEighthToOrthogonalSeq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalSeq0 = Narrow::kRightEighthToOrthogonalSeq0;
        constexpr uint16_t kRightEighthToOrthogonalSeq1 = Narrow::kRightEighthToOrthogonalSeq1;
        constexpr uint16_t kRightEighthToOrthogonalSeq2 = Narrow::kRightEighthToOrthogonalSeq2;
        constexpr uint16_t kRightEighthToOrthogonalSeq3 = Narrow::kRightEighthToOrthogonalSeq3;
        constexpr uint16_t kRightEighthToOrthogonalSeq4 = Narrow::kRightEighthToOrthogonalSeq4;
        constexpr uint16_t kLeftEighthBankToDiagSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToDiagSeq1 = Narrow::kLeftEighthBankToDiagSeq1;
        constexpr uint16_t kLeftEighthBankToDiagSeq2 = Narrow::kLeftEighthBankToDiagSeq2;
        constexpr uint16_t kLeftEighthBankToDiagSeq3 = Narrow::kLeftEighthBankToDiagSeq3;
        constexpr uint16_t kLeftEighthBankToDiagSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToDiagSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightEighthBankToDiagSeq1 = Narrow::kRightEighthBankToDiagSeq1;
        constexpr uint16_t kRightEighthBankToDiagSeq2 = Narrow::kRightEighthBankToDiagSeq2;
        constexpr uint16_t kRightEighthBankToDiagSeq3 = Narrow::kRightEighthBankToDiagSeq3;
        constexpr uint16_t kRightEighthBankToDiagSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq1 = Narrow::kLeftEighthBankToOrthogonalSeq1;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq2 = Narrow::kLeftEighthBankToOrthogonalSeq2;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq3 = Narrow::kLeftEighthBankToOrthogonalSeq3;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthBankToOrthogonalSeq1 = Narrow::kRightEighthBankToOrthogonalSeq1;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq2 = Narrow::kRightEighthBankToOrthogonalSeq2;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq3 = Narrow::kRightEighthBankToOrthogonalSeq3;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kDiagFlatSeq0 = Narrow::kDiagFlatSeq0;
        constexpr uint16_t kDiagFlatSeq1 = Narrow::kDiagFlatSeq1;
        constexpr uint16_t kDiagFlatSeq2 = Narrow::kDiagFlatSeq2;
        constexpr uint16_t kDiagFlatSeq3 = Narrow::kDiagFlatSeq3;
        constexpr uint16_t kDiagUp25Seq0 = Narrow::kDiagUp25Seq0;
        constexpr uint16_t kDiagUp25Seq1 = Narrow::kDiagUp25Seq1;
        constexpr uint16_t kDiagUp25Seq2 = Narrow::kDiagUp25Seq2;
        constexpr uint16_t kDiagUp25Seq3 = Narrow::kDiagUp25Seq3;
        constexpr uint16_t kDiagUp60Seq0 = Narrow::kDiagUp60Seq0;
        constexpr uint16_t kDiagUp60Seq1 = Narrow::kDiagUp60Seq1;
        constexpr uint16_t kDiagUp60Seq2 = Narrow::kDiagUp60Seq2;
        constexpr uint16_t kDiagUp60Seq3 = Narrow::kDiagUp60Seq3;
        constexpr uint16_t kDiagFlatToUp25Seq0 = Narrow::kDiagFlatToUp25Seq0;
        constexpr uint16_t kDiagFlatToUp25Seq1 = Narrow::kDiagFlatToUp25Seq1;
        constexpr uint16_t kDiagFlatToUp25Seq2 = Narrow::kDiagFlatToUp25Seq2;
        constexpr uint16_t kDiagFlatToUp25Seq3 = Narrow::kDiagFlatToUp25Seq3;
        constexpr uint16_t kDiagUp25ToUp60Seq0 = Narrow::kDiagUp25ToUp60Seq0;
        constexpr uint16_t kDiagUp25ToUp60Seq1 = Narrow::kDiagUp25ToUp60Seq1;
        constexpr uint16_t kDiagUp25ToUp60Seq2 = Narrow::kDiagUp25ToUp60Seq2;
        constexpr uint16_t kDiagUp25ToUp60Seq3 = Narrow::kDiagUp25ToUp60Seq3;
        constexpr uint16_t kDiagUp60ToUp25Seq0 = Narrow::kDiagUp60ToUp25Seq0;
        constexpr uint16_t kDiagUp60ToUp25Seq1 = Narrow::kDiagUp60ToUp25Seq1;
        constexpr uint16_t kDiagUp60ToUp25Seq2 = Narrow::kDiagUp60ToUp25Seq2;
        constexpr uint16_t kDiagUp60ToUp25Seq3 = Narrow::kDiagUp60ToUp25Seq3;
        constexpr uint16_t kDiagUp25ToFlatSeq0 = Narrow::kDiagUp25ToFlatSeq0;
        constexpr uint16_t kDiagUp25ToFlatSeq1 = Narrow::kDiagUp25ToFlatSeq1;
        constexpr uint16_t kDiagUp25ToFlatSeq2 = Narrow::kDiagUp25ToFlatSeq2;
        constexpr uint16_t kDiagUp25ToFlatSeq3 = Narrow::kDiagUp25ToFlatSeq3;
        constexpr uint16_t kDiagDown25Seq0 = Narrow::kDiagDown25Seq0;
        constexpr uint16_t kDiagDown25Seq1 = Narrow::kDiagDown25Seq1;
        constexpr uint16_t kDiagDown25Seq2 = Narrow::kDiagDown25Seq2;
        constexpr uint16_t kDiagDown25Seq3 = Narrow::kDiagDown25Seq3;
        constexpr uint16_t kDiagDown60Seq0 = Narrow::kDiagDown60Seq0;
        constexpr uint16_t kDiagDown60Seq1 = Narrow::kDiagDown60Seq1;
        constexpr uint16_t kDiagDown60Seq2 = Narrow::kDiagDown60Seq2;
        constexpr uint16_t kDiagDown60Seq3 = Narrow::kDiagDown60Seq3;
        constexpr uint16_t kDiagFlatToDown25Seq0 = Narrow::kDiagFlatToDown25Seq0;
        constexpr uint16_t kDiagFlatToDown25Seq1 = Narrow::kDiagFlatToDown25Seq1;
        constexpr uint16_t kDiagFlatToDown25Seq2 = Narrow::kDiagFlatToDown25Seq2;
        constexpr uint16_t kDiagFlatToDown25Seq3 = Narrow::kDiagFlatToDown25Seq3;
        constexpr uint16_t kDiagDown25ToDown60Seq0 = Narrow::kDiagDown25ToDown60Seq0;
        constexpr uint16_t kDiagDown25ToDown60Seq1 = Narrow::kDiagDown25ToDown60Seq1;
        constexpr uint16_t kDiagDown25ToDown60Seq2 = Narrow::kDiagDown25ToDown60Seq2;
        constexpr uint16_t kDiagDown25ToDown60Seq3 = Narrow::kDiagDown25ToDown60Seq3;
        constexpr uint16_t kDiagDown60ToDown25Seq0 = Narrow::kDiagDown60ToDown25Seq0;
        constexpr uint16_t kDiagDown60ToDown25Seq1 = Narrow::kDiagDown60ToDown25Seq1;
        constexpr uint16_t kDiagDown60ToDown25Seq2 = Narrow::kDiagDown60ToDown25Seq2;
        constexpr uint16_t kDiagDown60ToDown25Seq3 = Narrow::kDiagDown60ToDown25Seq3;
        constexpr uint16_t kDiagDown25ToFlatSeq0 = Narrow::kDiagDown25ToFlatSeq0;
        constexpr uint16_t kDiagDown25ToFlatSeq1 = Narrow::kDiagDown25ToFlatSeq1;
        constexpr uint16_t kDiagDown25ToFlatSeq2 = Narrow::kDiagDown25ToFlatSeq2;
        constexpr uint16_t kDiagDown25ToFlatSeq3 = Narrow::kDiagDown25ToFlatSeq3;
        constexpr uint16_t kDiagFlatToUp60Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankSeq0 = Narrow::kDiagFlatToLeftBankSeq0;
        constexpr uint16_t kDiagFlatToLeftBankSeq1 = Narrow::kDiagFlatToLeftBankSeq1;
        constexpr uint16_t kDiagFlatToLeftBankSeq2 = Narrow::kDiagFlatToLeftBankSeq2;
        constexpr uint16_t kDiagFlatToLeftBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagFlatToRightBankSeq0 = Narrow::kDiagFlatToRightBankSeq0;
        constexpr uint16_t kDiagFlatToRightBankSeq1 = Narrow::kDiagFlatToRightBankSeq1;
        constexpr uint16_t kDiagFlatToRightBankSeq2 = Narrow::kDiagFlatToRightBankSeq2;
        constexpr uint16_t kDiagFlatToRightBankSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankToFlatSeq0 = Narrow::kDiagLeftBankToFlatSeq0;
        constexpr uint16_t kDiagLeftBankToFlatSeq1 = Narrow::kDiagLeftBankToFlatSeq1;
        constexpr uint16_t kDiagLeftBankToFlatSeq2 = Narrow::kDiagLeftBankToFlatSeq2;
        constexpr uint16_t kDiagLeftBankToFlatSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankToFlatSeq0 = Narrow::kDiagRightBankToFlatSeq0;
        constexpr uint16_t kDiagRightBankToFlatSeq1 = Narrow::kDiagRightBankToFlatSeq1;
        constexpr uint16_t kDiagRightBankToFlatSeq2 = Narrow::kDiagRightBankToFlatSeq2;
        constexpr uint16_t kDiagRightBankToFlatSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagLeftBankToUp25Seq0 = Narrow::kDiagLeftBankToUp25Seq0;
        constexpr uint16_t kDiagLeftBankToUp25Seq1 = Narrow::kDiagLeftBankToUp25Seq1;
        constexpr uint16_t kDiagLeftBankToUp25Seq2 = Narrow::kDiagLeftBankToUp25Seq2;
        constexpr uint16_t kDiagLeftBankToUp25Seq3 = Narrow::kDiagLeftBankToUp25Seq3;
        constexpr uint16_t kDiagRightBankToUp25Seq0 = Narrow::kDiagRightBankToUp25Seq0;
        constexpr uint16_t kDiagRightBankToUp25Seq1 = Narrow::kDiagRightBankToUp25Seq1;
        constexpr uint16_t kDiagRightBankToUp25Seq2 = Narrow::kDiagRightBankToUp25Seq2;
        constexpr uint16_t kDiagRightBankToUp25Seq3 = Narrow::kDiagRightBankToUp25Seq3;
        constexpr uint16_t kDiagUp25ToLeftBankSeq0 = Narrow::kDiagUp25ToLeftBankSeq0;
        constexpr uint16_t kDiagUp25ToLeftBankSeq1 = Narrow::kDiagUp25ToLeftBankSeq1;
        constexpr uint16_t kDiagUp25ToLeftBankSeq2 = Narrow::kDiagUp25ToLeftBankSeq2;
        constexpr uint16_t kDiagUp25ToLeftBankSeq3 = Narrow::kDiagUp25ToLeftBankSeq3;
        constexpr uint16_t kDiagUp25ToRightBankSeq0 = Narrow::kDiagUp25ToRightBankSeq0;
        constexpr uint16_t kDiagUp25ToRightBankSeq1 = Narrow::kDiagUp25ToRightBankSeq1;
        constexpr uint16_t kDiagUp25ToRightBankSeq2 = Narrow::kDiagUp25ToRightBankSeq2;
        constexpr uint16_t kDiagUp25ToRightBankSeq3 = Narrow::kDiagUp25ToRightBankSeq3;
        constexpr uint16_t kDiagLeftBankToDown25Seq0 = Narrow::kDiagLeftBankToDown25Seq0;
        constexpr uint16_t kDiagLeftBankToDown25Seq1 = Narrow::kDiagLeftBankToDown25Seq1;
        constexpr uint16_t kDiagLeftBankToDown25Seq2 = Narrow::kDiagLeftBankToDown25Seq2;
        constexpr uint16_t kDiagLeftBankToDown25Seq3 = Narrow::kDiagLeftBankToDown25Seq3;
        constexpr uint16_t kDiagRightBankToDown25Seq0 = Narrow::kDiagRightBankToDown25Seq0;
        constexpr uint16_t kDiagRightBankToDown25Seq1 = Narrow::kDiagRightBankToDown25Seq1;
        constexpr uint16_t kDiagRightBankToDown25Seq2 = Narrow::kDiagRightBankToDown25Seq2;
        constexpr uint16_t kDiagRightBankToDown25Seq3 = Narrow::kDiagRightBankToDown25Seq3;
        constexpr uint16_t kDiagDown25ToLeftBankSeq0 = Narrow::kDiagDown25ToLeftBankSeq0;
        constexpr uint16_t kDiagDown25ToLeftBankSeq1 = Narrow::kDiagDown25ToLeftBankSeq1;
        constexpr uint16_t kDiagDown25ToLeftBankSeq2 = Narrow::kDiagDown25ToLeftBankSeq2;
        constexpr uint16_t kDiagDown25ToLeftBankSeq3 = Narrow::kDiagDown25ToLeftBankSeq3;
        constexpr uint16_t kDiagDown25ToRightBankSeq0 = Narrow::kDiagDown25ToRightBankSeq0;
        constexpr uint16_t kDiagDown25ToRightBankSeq1 = Narrow::kDiagDown25ToRightBankSeq1;
        constexpr uint16_t kDiagDown25ToRightBankSeq2 = Narrow::kDiagDown25ToRightBankSeq2;
        constexpr uint16_t kDiagDown25ToRightBankSeq3 = Narrow::kDiagDown25ToRightBankSeq3;
        constexpr uint16_t kDiagLeftBankSeq0 = Narrow::kDiagLeftBankSeq0;
        constexpr uint16_t kDiagLeftBankSeq1 = Narrow::kDiagLeftBankSeq1;
        constexpr uint16_t kDiagLeftBankSeq2 = Narrow::kDiagLeftBankSeq2;
        constexpr uint16_t kDiagLeftBankSeq3 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kDiagRightBankSeq0 = Narrow::kDiagRightBankSeq0;
        constexpr uint16_t kDiagRightBankSeq1 = Narrow::kDiagRightBankSeq1;
        constexpr uint16_t kDiagRightBankSeq2 = Narrow::kDiagRightBankSeq2;
        constexpr uint16_t kDiagRightBankSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLogFlumeReverserSeq0 = kFlatSeq0;
        constexpr uint16_t kSpinningTunnelSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBarrelRollUpToDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBarrelRollUpToDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBarrelRollUpToDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBarrelRollDownToUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBarrelRollDownToUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBarrelRollDownToUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBarrelRollDownToUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBarrelRollDownToUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBarrelRollDownToUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kPoweredLiftSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq2 = Narrow::kLeftLargeHalfLoopUpSeq2;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq3 = Narrow::kLeftLargeHalfLoopUpSeq3;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq4 = Narrow::kLeftLargeHalfLoopUpSeq4;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq5 = Narrow::kLeftLargeHalfLoopUpSeq5;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq6 = Narrow::kLeftLargeHalfLoopUpSeq6;
        constexpr uint16_t kRightLargeHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopUpSeq2 = Narrow::kRightLargeHalfLoopUpSeq2;
        constexpr uint16_t kRightLargeHalfLoopUpSeq3 = Narrow::kRightLargeHalfLoopUpSeq3;
        constexpr uint16_t kRightLargeHalfLoopUpSeq4 = Narrow::kRightLargeHalfLoopUpSeq4;
        constexpr uint16_t kRightLargeHalfLoopUpSeq5 = Narrow::kRightLargeHalfLoopUpSeq5;
        constexpr uint16_t kRightLargeHalfLoopUpSeq6 = Narrow::kRightLargeHalfLoopUpSeq6;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq0 = Narrow::kLeftLargeHalfLoopDownSeq0;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq1 = Narrow::kLeftLargeHalfLoopDownSeq1;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq2 = Narrow::kLeftLargeHalfLoopDownSeq2;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq3 = Narrow::kLeftLargeHalfLoopDownSeq3;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq4 = Narrow::kLeftLargeHalfLoopDownSeq4;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq5 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq6 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopDownSeq0 = Narrow::kRightLargeHalfLoopDownSeq0;
        constexpr uint16_t kRightLargeHalfLoopDownSeq1 = Narrow::kRightLargeHalfLoopDownSeq1;
        constexpr uint16_t kRightLargeHalfLoopDownSeq2 = Narrow::kRightLargeHalfLoopDownSeq2;
        constexpr uint16_t kRightLargeHalfLoopDownSeq3 = Narrow::kRightLargeHalfLoopDownSeq3;
        constexpr uint16_t kRightLargeHalfLoopDownSeq4 = Narrow::kRightLargeHalfLoopDownSeq4;
        constexpr uint16_t kRightLargeHalfLoopDownSeq5 = kFlatSeq0;
        constexpr uint16_t kRightLargeHalfLoopDownSeq6 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerTwistUpSeq0 = Narrow::kLeftFlyerTwistUpSeq0;
        constexpr uint16_t kLeftFlyerTwistUpSeq1 = Narrow::kLeftFlyerTwistUpSeq1;
        constexpr uint16_t kLeftFlyerTwistUpSeq2 = Narrow::kLeftFlyerTwistUpSeq2;
        constexpr uint16_t kRightFlyerTwistUpSeq0 = Narrow::kRightFlyerTwistUpSeq0;
        constexpr uint16_t kRightFlyerTwistUpSeq1 = Narrow::kRightFlyerTwistUpSeq1;
        constexpr uint16_t kRightFlyerTwistUpSeq2 = Narrow::kRightFlyerTwistUpSeq2;
        constexpr uint16_t kLeftFlyerTwistDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerTwistDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerTwistDownSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerTwistDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerTwistDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerTwistDownSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq0 = Narrow::kFlyerHalfLoopUninvertedUpSeq0;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq1 = Narrow::kFlyerHalfLoopUninvertedUpSeq1;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq2 = Narrow::kFlyerHalfLoopUninvertedUpSeq2;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq3 = Narrow::kFlyerHalfLoopUninvertedUpSeq3;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq0 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq1 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq2 = kSegmentsAll;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq0 = Narrow::kLeftFlyerCorkscrewUpSeq0;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq1 = Narrow::kLeftFlyerCorkscrewUpSeq1;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq2 = Narrow::kLeftFlyerCorkscrewUpSeq2;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq0 = Narrow::kRightFlyerCorkscrewUpSeq0;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq1 = Narrow::kRightFlyerCorkscrewUpSeq1;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq2 = Narrow::kRightFlyerCorkscrewUpSeq2;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerCorkscrewDownSeq0 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerCorkscrewDownSeq1 = kSegmentsAll;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kHeartLineTransferUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftHeartLineRollSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightHeartLineRollSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleASeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleASeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleBSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleBSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleCSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleCSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleDSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleDSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleDSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleESeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleESeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kMinigolfHoleESeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq0;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq1;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq2;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq3;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightReverserSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustTopCapSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kBlockBrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq1 = Narrow::kLeftBankedQuarterTurn3TileUp25Seq1;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq2 = Narrow::kLeftBankedQuarterTurn3TileUp25Seq2;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq1 = Narrow::kRightBankedQuarterTurn3TileUp25Seq1;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq2 = Narrow::kRightBankedQuarterTurn3TileUp25Seq2;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq1 = Narrow::kLeftBankedQuarterTurn3TileDown25Seq1;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq2 = Narrow::kLeftBankedQuarterTurn3TileDown25Seq2;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq3 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq1 = Narrow::kRightBankedQuarterTurn3TileDown25Seq1;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq2 = Narrow::kRightBankedQuarterTurn3TileDown25Seq2;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq3 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq1 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq1;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq2 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq2;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq3 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq3;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq4 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq4;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq5 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq5;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq1 = Narrow::kRightBankedQuarterTurn5TileUp25Seq1;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq2 = Narrow::kRightBankedQuarterTurn5TileUp25Seq2;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq3 = Narrow::kRightBankedQuarterTurn5TileUp25Seq3;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq4 = Narrow::kRightBankedQuarterTurn5TileUp25Seq4;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq5 = Narrow::kRightBankedQuarterTurn5TileUp25Seq5;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq0 = EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq1 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq1;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq2 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq2;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq3 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq3;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq4 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq4;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq5 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq5;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq6 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq0 = EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq1 = Narrow::kRightBankedQuarterTurn5TileDown25Seq1;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq2 = Narrow::kRightBankedQuarterTurn5TileDown25Seq2;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq3 = Narrow::kRightBankedQuarterTurn5TileDown25Seq3;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq4 = Narrow::kRightBankedQuarterTurn5TileDown25Seq4;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq5 = Narrow::kRightBankedQuarterTurn5TileDown25Seq5;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq6 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kUp25ToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedFlatToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedFlatToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToLeftBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToRightBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedFlatToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedFlatToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToLeftBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToRightBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn1TileUp90Seq0 = Narrow::kLeftQuarterTurn1TileUp90Seq0;
        constexpr uint16_t kLeftQuarterTurn1TileUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn1TileUp90Seq0 = Narrow::kRightQuarterTurn1TileUp90Seq0;
        constexpr uint16_t kRightQuarterTurn1TileUp90Seq1 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterTurn1TileDown90Seq0 = Narrow::kLeftQuarterTurn1TileDown90Seq0;
        constexpr uint16_t kLeftQuarterTurn1TileDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kRightQuarterTurn1TileDown90Seq0 = Narrow::kRightQuarterTurn1TileDown90Seq0;
        constexpr uint16_t kRightQuarterTurn1TileDown90Seq1 = kSegmentsNone;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = Narrow::kMultiDimUp90ToInvertedFlatQuarterLoopSeq0;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = Narrow::kMultiDimUp90ToInvertedFlatQuarterLoopSeq1;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = Narrow::kMultiDimUp90ToInvertedFlatQuarterLoopSeq2;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq0 = Narrow::kMultiDimFlatToDown90QuarterLoopSeq0;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq1 = Narrow::kMultiDimFlatToDown90QuarterLoopSeq1;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq2 = Narrow::kMultiDimFlatToDown90QuarterLoopSeq2;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq3 = Narrow::kMultiDimFlatToDown90QuarterLoopSeq3;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = Narrow::kMultiDimInvertedUp90ToFlatQuarterLoopSeq0;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = Narrow::kMultiDimInvertedUp90ToFlatQuarterLoopSeq1;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = Narrow::kMultiDimInvertedUp90ToFlatQuarterLoopSeq2;
        constexpr uint16_t kRotationControlToggleSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatTrack1x4ASeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq8 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq9 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq10 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq11 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq12 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq13 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq14 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq15 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x1ASeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x1BSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq8 = kSegmentsNone;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightMediumHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq4 = kFlatSeq0;
        constexpr uint16_t kRightMediumHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq4 = kFlatSeq0;
        constexpr uint16_t kLeftZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftZeroGRollUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftZeroGRollUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightZeroGRollUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightZeroGRollUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftZeroGRollDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftZeroGRollDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kRightZeroGRollDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightZeroGRollDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeZeroGRollUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeZeroGRollUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq3 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeZeroGRollDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeZeroGRollDownSeq2 = kFlatSeq0;
        constexpr uint16_t kRightLargeZeroGRollDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq5 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq6 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq5 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq6 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq1 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq2 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq4 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq5 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq6 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq1 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq2 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq3 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq4 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq5 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq6 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = kFlatSeq0;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq5 = kFlatSeq0;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq6 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq0 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq1 = kSegmentsAll;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq3 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq0 = kFlatSeq0;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq3 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagUp25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagUp25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagDown25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToDiagDown25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25LeftBankedSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25LeftBankedSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25LeftBankedSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25LeftBankedSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25RightBankedSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25RightBankedSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25RightBankedSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp25RightBankedSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25LeftBankedSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25LeftBankedSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25LeftBankedSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25LeftBankedSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25RightBankedSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25RightBankedSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25RightBankedSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25RightBankedSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagBrakesSeq0 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBrakesSeq1 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBrakesSeq2 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBrakesSeq3 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq0 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq1 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq2 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq3 = kDiagFlatSeq0;
        constexpr uint16_t kDown25BrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq0 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq1 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq2 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq3 = kDiagFlatSeq0;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25BrakesSeq0 = kDiagDown25Seq0;
        constexpr uint16_t kDiagDown25BrakesSeq1 = kDiagDown25Seq1;
        constexpr uint16_t kDiagDown25BrakesSeq2 = kDiagDown25Seq2;
        constexpr uint16_t kDiagDown25BrakesSeq3 = kDiagDown25Seq3;
    } // namespace Inverted

    namespace Wide
    {
        constexpr uint16_t kFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kEndStationSeq0 = kSegmentsAll;
        constexpr uint16_t kBeginStationSeq0 = kSegmentsAll;
        constexpr uint16_t kMiddleStationSeq0 = kSegmentsAll;
        constexpr uint16_t kUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq2 = Narrow::kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftQuarterTurn5TilesSeq5 = Narrow::kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesSeq6 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn5TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn5TilesSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq2 = Narrow::kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightQuarterTurn5TilesSeq5 = Narrow::kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesSeq6 = kSegmentsAll;
        constexpr uint16_t kFlatToLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq2 = Narrow::kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq5 = Narrow::kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kBankedLeftQuarterTurn5TilesSeq6 = kSegmentsAll;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq2 = Narrow::kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq5 = Narrow::kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kBankedRightQuarterTurn5TilesSeq6 = kSegmentsAll;
        constexpr uint16_t kLeftBankToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesUp25Seq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesUp25Seq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesDown25Seq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesDown25Seq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kSBendLeftSeq0 = kSegmentsAll;
        constexpr uint16_t kSBendLeftSeq1 = Narrow::kSBendLeftSeq1;
        constexpr uint16_t kSBendLeftSeq2 = Narrow::kSBendLeftSeq2;
        constexpr uint16_t kSBendLeftSeq3 = kSegmentsAll;
        constexpr uint16_t kSBendRightSeq0 = kSegmentsAll;
        constexpr uint16_t kSBendRightSeq1 = Narrow::kSBendRightSeq1;
        constexpr uint16_t kSBendRightSeq2 = Narrow::kSBendRightSeq2;
        constexpr uint16_t kSBendRightSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftVerticalLoopSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftVerticalLoopSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kRightVerticalLoopSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq1 = Narrow::kLeftQuarterTurn3TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq2 = Narrow::kLeftQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn3TilesSeq3 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesSeq1 = Narrow::kRightQuarterTurn3TilesSeq1;
        constexpr uint16_t kRightQuarterTurn3TilesSeq2 = Narrow::kRightQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightQuarterTurn3TilesSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq2 = Narrow::kLeftBankedQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftBankedQuarterTurn3TilesSeq3 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq0 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq2 = Narrow::kRightBankedQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightBankedQuarterTurn3TilesSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq1 = Narrow::kLeftQuarterTurn3TilesUp25Seq1;
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq1 = Narrow::kRightQuarterTurn3TilesUp25Seq1;
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq1 = Narrow::kLeftQuarterTurn3TilesDown25Seq1;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq1 = Narrow::kRightQuarterTurn3TilesDown25Seq1;
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn1TileSeq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn1TileSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftTwistDownToUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftTwistDownToUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftTwistDownToUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightTwistDownToUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightTwistDownToUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightTwistDownToUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftTwistUpToDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftTwistUpToDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftTwistUpToDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightTwistUpToDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightTwistUpToDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightTwistUpToDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlatToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kTowerBaseSeq0 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq1 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq2 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq3 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq4 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq5 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq6 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq7 = kSegmentsNone;
        constexpr uint16_t kTowerBaseSeq8 = kSegmentsNone;
        constexpr uint16_t kTowerSectionSeq0 = kSegmentsNone;
        constexpr uint16_t kTowerSectionSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatCoveredSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25CoveredSeq0 = kUp25Seq0;
        constexpr uint16_t kUp60CoveredSeq0 = kUp60Seq0;
        constexpr uint16_t kFlatToUp25CoveredSeq0 = kFlatToUp25Seq0;
        constexpr uint16_t kUp25ToUp60CoveredSeq0 = kUp25ToUp60Seq0;
        constexpr uint16_t kUp60ToUp25CoveredSeq0 = kUp60ToUp25Seq0;
        constexpr uint16_t kUp25ToFlatCoveredSeq0 = kUp25ToFlatSeq0;
        constexpr uint16_t kDown25CoveredSeq0 = kDown25Seq0;
        constexpr uint16_t kDown60CoveredSeq0 = kDown60Seq0;
        constexpr uint16_t kFlatToDown25CoveredSeq0 = kFlatToDown25Seq0;
        constexpr uint16_t kDown25ToDown60CoveredSeq0 = kDown25ToDown60Seq0;
        constexpr uint16_t kDown60ToDown25CoveredSeq0 = kDown60ToDown25Seq0;
        constexpr uint16_t kDown25ToFlatCoveredSeq0 = kDown25ToFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterTurn5TilesCoveredSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterTurn5TilesCoveredSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kSBendLeftCoveredSeq0 = kSBendLeftSeq0;
        constexpr uint16_t kSBendLeftCoveredSeq1 = kSBendLeftSeq1;
        constexpr uint16_t kSBendLeftCoveredSeq2 = kSBendLeftSeq2;
        constexpr uint16_t kSBendLeftCoveredSeq3 = kSBendLeftSeq3;
        constexpr uint16_t kSBendRightCoveredSeq0 = kSBendRightSeq0;
        constexpr uint16_t kSBendRightCoveredSeq1 = kSBendRightSeq1;
        constexpr uint16_t kSBendRightCoveredSeq2 = kSBendRightSeq2;
        constexpr uint16_t kSBendRightCoveredSeq3 = kSBendRightSeq3;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq0 = kLeftQuarterTurn3TilesSeq0;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq1 = kLeftQuarterTurn3TilesSeq1;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq2 = kLeftQuarterTurn3TilesSeq2;
        constexpr uint16_t kLeftQuarterTurn3TilesCoveredSeq3 = kLeftQuarterTurn3TilesSeq3;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq0 = kRightQuarterTurn3TilesSeq0;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq1 = kRightQuarterTurn3TilesSeq1;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq2 = kRightQuarterTurn3TilesSeq2;
        constexpr uint16_t kRightQuarterTurn3TilesCoveredSeq3 = kRightQuarterTurn3TilesSeq3;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq1 = Narrow::kLeftHalfBankedHelixUpSmallSeq1;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq2 = Narrow::kLeftHalfBankedHelixUpSmallSeq2;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq5 = Narrow::kLeftHalfBankedHelixUpSmallSeq5;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq6 = Narrow::kLeftHalfBankedHelixUpSmallSeq6;
        constexpr uint16_t kLeftHalfBankedHelixUpSmallSeq7 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq0 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq1 = Narrow::kRightHalfBankedHelixUpSmallSeq1;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq2 = Narrow::kRightHalfBankedHelixUpSmallSeq2;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq3 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq4 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq5 = Narrow::kRightHalfBankedHelixUpSmallSeq5;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq6 = Narrow::kRightHalfBankedHelixUpSmallSeq6;
        constexpr uint16_t kRightHalfBankedHelixUpSmallSeq7 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq1 = Narrow::kLeftHalfBankedHelixDownSmallSeq1;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq2 = Narrow::kLeftHalfBankedHelixDownSmallSeq2;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq5 = Narrow::kLeftHalfBankedHelixDownSmallSeq5;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq6 = Narrow::kLeftHalfBankedHelixDownSmallSeq6;
        constexpr uint16_t kLeftHalfBankedHelixDownSmallSeq7 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq0 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq1 = Narrow::kRightHalfBankedHelixDownSmallSeq1;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq2 = Narrow::kRightHalfBankedHelixDownSmallSeq2;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq3 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq4 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq5 = Narrow::kRightHalfBankedHelixDownSmallSeq5;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq6 = Narrow::kRightHalfBankedHelixDownSmallSeq6;
        constexpr uint16_t kRightHalfBankedHelixDownSmallSeq7 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq2 = Narrow::kLeftHalfBankedHelixUpLargeSeq2;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq5 = Narrow::kLeftHalfBankedHelixUpLargeSeq5;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq6 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq7 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq8 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq9 = Narrow::kLeftHalfBankedHelixUpLargeSeq9;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq10 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq11 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq12 = Narrow::kLeftHalfBankedHelixUpLargeSeq12;
        constexpr uint16_t kLeftHalfBankedHelixUpLargeSeq13 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq0 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq2 = Narrow::kRightHalfBankedHelixUpLargeSeq2;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq5 = Narrow::kRightHalfBankedHelixUpLargeSeq5;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq6 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq7 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq8 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq9 = Narrow::kRightHalfBankedHelixUpLargeSeq9;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq10 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq11 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq12 = Narrow::kRightHalfBankedHelixUpLargeSeq12;
        constexpr uint16_t kRightHalfBankedHelixUpLargeSeq13 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq2 = Narrow::kLeftHalfBankedHelixDownLargeSeq2;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq5 = Narrow::kLeftHalfBankedHelixDownLargeSeq5;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq6 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq7 = kSegmentsAll;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq8 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq9 = Narrow::kLeftHalfBankedHelixDownLargeSeq9;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq10 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq11 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq12 = Narrow::kLeftHalfBankedHelixDownLargeSeq12;
        constexpr uint16_t kLeftHalfBankedHelixDownLargeSeq13 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq0 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq2 = Narrow::kRightHalfBankedHelixDownLargeSeq2;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq5 = Narrow::kRightHalfBankedHelixDownLargeSeq5;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq6 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq7 = kSegmentsAll;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq8 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq9 = Narrow::kRightHalfBankedHelixDownLargeSeq9;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq10 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq11 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq12 = Narrow::kRightHalfBankedHelixDownLargeSeq12;
        constexpr uint16_t kRightHalfBankedHelixDownLargeSeq13 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn1TileUp60Seq0 = Narrow::kLeftQuarterTurn1TileUp60Seq0;
        constexpr uint16_t kRightQuarterTurn1TileUp60Seq0 = Narrow::kRightQuarterTurn1TileUp60Seq0;
        constexpr uint16_t kLeftQuarterTurn1TileDown60Seq0 = Narrow::kLeftQuarterTurn1TileDown60Seq0;
        constexpr uint16_t kRightQuarterTurn1TileDown60Seq0 = Narrow::kRightQuarterTurn1TileDown60Seq0;
        constexpr uint16_t kBrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kBoosterSeq0 = kFlatSeq0;
        constexpr uint16_t kMazeSeq0 = kSegmentsNone;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq0 = kBankedLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq1 = kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq2 = kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq3 = kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq4 = kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq5 = kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterBankedHelixLargeUpSeq6 = kBankedLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq0 = kBankedRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq1 = kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq2 = kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq3 = kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq4 = kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq5 = kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterBankedHelixLargeUpSeq6 = kBankedRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq0 = kBankedLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq1 = kBankedLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq2 = kBankedLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq3 = kBankedLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq4 = kBankedLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq5 = kBankedLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterBankedHelixLargeDownSeq6 = kBankedLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq0 = kBankedRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq1 = kBankedRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq2 = kBankedRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq3 = kBankedRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq4 = kBankedRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq5 = kBankedRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterBankedHelixLargeDownSeq6 = kBankedRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterHelixLargeUpSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterHelixLargeUpSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq0 = kLeftQuarterTurn5TilesSeq0;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq1 = kLeftQuarterTurn5TilesSeq1;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq2 = kLeftQuarterTurn5TilesSeq2;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq3 = kLeftQuarterTurn5TilesSeq3;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq4 = kLeftQuarterTurn5TilesSeq4;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq5 = kLeftQuarterTurn5TilesSeq5;
        constexpr uint16_t kLeftQuarterHelixLargeDownSeq6 = kLeftQuarterTurn5TilesSeq6;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq0 = kRightQuarterTurn5TilesSeq0;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq1 = kRightQuarterTurn5TilesSeq1;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq2 = kRightQuarterTurn5TilesSeq2;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq3 = kRightQuarterTurn5TilesSeq3;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq4 = kRightQuarterTurn5TilesSeq4;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq5 = kRightQuarterTurn5TilesSeq5;
        constexpr uint16_t kRightQuarterHelixLargeDownSeq6 = kRightQuarterTurn5TilesSeq6;
        constexpr uint16_t kUp25LeftBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kUp25RightBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kWaterfallSeq0 = kFlatSeq0;
        constexpr uint16_t kRapidsSeq0 = kFlatSeq0;
        constexpr uint16_t kOnRidePhotoSeq0 = Narrow::kOnRidePhotoSeq0;
        constexpr uint16_t kDown25LeftBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kDown25RightBankedSeq0 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq0 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq1 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq2 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq3 = kFlatSeq0;
        constexpr uint16_t kWatersplashSeq4 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kFlatToUp60LongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kUp60ToFlatLongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kWhirlpoolSeq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kDown60ToFlatLongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq1 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq2 = kFlatSeq0;
        constexpr uint16_t kFlatToDown60LongBaseSeq3 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq0 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq1 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq2 = kFlatSeq0;
        constexpr uint16_t kCableLiftHillSeq3 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq0 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq1 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq2 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq3 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq4 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq5 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallSlopeSeq6 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallVerticalSeq0 = kFlatSeq0;
        constexpr uint16_t kReverseFreefallVerticalSeq1 = kFlatSeq0;
        constexpr uint16_t kUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kUp90Seq1 = Narrow::kUp90Seq1;
        constexpr uint16_t kDown90Seq0 = kFlatSeq0;
        constexpr uint16_t kDown90Seq1 = Narrow::kDown90Seq1;
        constexpr uint16_t kUp60ToUp90Seq0 = kFlatSeq0;
        constexpr uint16_t kUp60ToUp90Seq1 = Narrow::kUp60ToUp90Seq1;
        constexpr uint16_t kDown90ToDown60Seq0 = kFlatSeq0;
        constexpr uint16_t kUp90ToUp60Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown90Seq0 = kFlatSeq0;
        constexpr uint16_t kDown60ToDown90Seq1 = Narrow::kDown60ToDown90Seq1;
        constexpr uint16_t kBrakeForDropSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagSeq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagSeq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagSeq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagSeq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagSeq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalSeq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalSeq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalSeq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalSeq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalSeq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagSeq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagSeq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagSeq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagSeq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagSeq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalSeq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalSeq4 = kSegmentsAll;
        constexpr uint16_t kDiagFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp60Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp60Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp60Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp60Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToUp60Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToUp60Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToUp60Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToUp60Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown60Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown60Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown60Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown60Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToDown60Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToDown60Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToDown60Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToDown60Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp60Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp60Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp60Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToUp60Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp60ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown60Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown60Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown60Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToDown60Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown60ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankToDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankToDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankSeq3 = kSegmentsAll;
        constexpr uint16_t kLogFlumeReverserSeq0 = kFlatSeq0;
        constexpr uint16_t kSpinningTunnelSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollUpToDownSeq2 = kSegmentsAll;
        constexpr uint16_t kRightBarrelRollUpToDownSeq0 = kSegmentsAll;
        constexpr uint16_t kRightBarrelRollUpToDownSeq1 = kSegmentsAll;
        constexpr uint16_t kRightBarrelRollUpToDownSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollDownToUpSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollDownToUpSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftBarrelRollDownToUpSeq2 = kSegmentsAll;
        constexpr uint16_t kRightBarrelRollDownToUpSeq0 = kSegmentsAll;
        constexpr uint16_t kRightBarrelRollDownToUpSeq1 = kSegmentsAll;
        constexpr uint16_t kRightBarrelRollDownToUpSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq1;
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq1 = Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq1;
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankToRightQuarterTurn3TilesUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq1;
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq1 = Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq1;
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightQuarterTurn3TilesDown25ToRightBankSeq3 = kSegmentsAll;
        constexpr uint16_t kPoweredLiftSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeHalfLoopDownSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeHalfLoopDownSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerTwistDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerTwistDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kHeartLineTransferUpSeq0 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferUpSeq1 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferUpSeq2 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferUpSeq3 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferDownSeq0 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferDownSeq1 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferDownSeq2 = kSegmentsAll;
        constexpr uint16_t kHeartLineTransferDownSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftHeartLineRollSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftHeartLineRollSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftHeartLineRollSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftHeartLineRollSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftHeartLineRollSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftHeartLineRollSeq5 = kSegmentsAll;
        constexpr uint16_t kRightHeartLineRollSeq0 = kSegmentsAll;
        constexpr uint16_t kRightHeartLineRollSeq1 = kSegmentsAll;
        constexpr uint16_t kRightHeartLineRollSeq2 = kSegmentsAll;
        constexpr uint16_t kRightHeartLineRollSeq3 = kSegmentsAll;
        constexpr uint16_t kRightHeartLineRollSeq4 = kSegmentsAll;
        constexpr uint16_t kRightHeartLineRollSeq5 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleASeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleASeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleBSeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleBSeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleCSeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleCSeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleDSeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleDSeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleDSeq2 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleESeq0 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleESeq1 = kSegmentsAll;
        constexpr uint16_t kMinigolfHoleESeq2 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = kSegmentsNone;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq0 = kSegmentsAll;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kUp90ToInvertedFlatQuarterLoopSeq2 = kSegmentsAll;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq0 = kSegmentsAll;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq2 = kSegmentsAll;
        constexpr uint16_t kInvertedFlatToDown90QuarterLoopSeq3 = Narrow::kInvertedFlatToDown90QuarterLoopSeq3;
        constexpr uint16_t kLeftCurvedLiftHillSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftCurvedLiftHillSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightCurvedLiftHillSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftReverserSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftReverserSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftReverserSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftReverserSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftReverserSeq4 = kSegmentsAll;
        constexpr uint16_t kLeftReverserSeq5 = kSegmentsAll;
        constexpr uint16_t kRightReverserSeq0 = kSegmentsAll;
        constexpr uint16_t kRightReverserSeq1 = kSegmentsAll;
        constexpr uint16_t kRightReverserSeq2 = kSegmentsAll;
        constexpr uint16_t kRightReverserSeq3 = kSegmentsAll;
        constexpr uint16_t kRightReverserSeq4 = kSegmentsAll;
        constexpr uint16_t kRightReverserSeq5 = kSegmentsAll;
        constexpr uint16_t kAirThrustTopCapSeq0 = kSegmentsAll;
        constexpr uint16_t kAirThrustTopCapSeq1 = kSegmentsAll;
        constexpr uint16_t kAirThrustTopCapSeq2 = kSegmentsAll;
        constexpr uint16_t kAirThrustTopCapSeq3 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownSeq0 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownSeq1 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq0 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq1 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq2 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq3 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq4 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq5 = kSegmentsAll;
        constexpr uint16_t kAirThrustVerticalDownToLevelSeq6 = kSegmentsAll;
        constexpr uint16_t kBlockBrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq1 = Narrow::kLeftBankedQuarterTurn3TileUp25Seq1;
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn3TileUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq1 = Narrow::kRightBankedQuarterTurn3TileUp25Seq1;
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TileUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq1 = Narrow::kLeftBankedQuarterTurn3TileDown25Seq1;
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq2 = EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft);
        constexpr uint16_t kLeftBankedQuarterTurn3TileDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq1 = Narrow::kRightBankedQuarterTurn3TileDown25Seq1;
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq2 = EnumsToFlags(PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn3TileDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq2 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq2;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq4 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq5 = Narrow::kLeftBankedQuarterTurn5TileUp25Seq5;
        constexpr uint16_t kLeftBankedQuarterTurn5TileUp25Seq6 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq2 = Narrow::kRightBankedQuarterTurn5TileUp25Seq2;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq4 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq5 = Narrow::kRightBankedQuarterTurn5TileUp25Seq5;
        constexpr uint16_t kRightBankedQuarterTurn5TileUp25Seq6 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq1 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq2 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq2;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq4 = EnumsToFlags(PaintSegment::right, PaintSegment::topRight, PaintSegment::bottomRight);
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq5 = Narrow::kLeftBankedQuarterTurn5TileDown25Seq5;
        constexpr uint16_t kLeftBankedQuarterTurn5TileDown25Seq6 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq1 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq2 = Narrow::kRightBankedQuarterTurn5TileDown25Seq2;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq3 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq4 = EnumsToFlags(PaintSegment::top, PaintSegment::topLeft, PaintSegment::topRight);
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq5 = Narrow::kRightBankedQuarterTurn5TileDown25Seq5;
        constexpr uint16_t kRightBankedQuarterTurn5TileDown25Seq6 = kSegmentsAll;
        constexpr uint16_t kUp25ToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kUp25ToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kDown25ToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedFlatToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedFlatToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToLeftBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToRightBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedFlatToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedFlatToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToLeftBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToRightBankedFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToLeftBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankedUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedUp25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatToLeftBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kFlatToRightBankedDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftBankedDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kRightBankedDown25ToFlatSeq0 = kFlatSeq0;
        constexpr uint16_t kLeftQuarterTurn1TileUp90Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn1TileUp90Seq1 = Narrow::kLeftQuarterTurn1TileUp90Seq1;
        constexpr uint16_t kRightQuarterTurn1TileUp90Seq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn1TileUp90Seq1 = Narrow::kRightQuarterTurn1TileUp90Seq1;
        constexpr uint16_t kLeftQuarterTurn1TileDown90Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftQuarterTurn1TileDown90Seq1 = Narrow::kLeftQuarterTurn1TileDown90Seq1;
        constexpr uint16_t kRightQuarterTurn1TileDown90Seq0 = kSegmentsAll;
        constexpr uint16_t kRightQuarterTurn1TileDown90Seq1 = Narrow::kRightQuarterTurn1TileDown90Seq1;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = kSegmentsAll;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = kSegmentsAll;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq0 = kSegmentsAll;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq2 = kSegmentsAll;
        constexpr uint16_t kMultiDimFlatToDown90QuarterLoopSeq3 = kSegmentsNone;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = kSegmentsAll;
        constexpr uint16_t kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = kSegmentsAll;
        constexpr uint16_t kRotationControlToggleSeq0 = kFlatSeq0;
        constexpr uint16_t kFlatTrack1x4ASeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4ASeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x2Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq8 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq9 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq10 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq11 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq12 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq13 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq14 = kSegmentsNone;
        constexpr uint16_t kFlatTrack4x4Seq15 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack2x4Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x5Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x1ASeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4BSeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x1BSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack1x4CSeq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq0 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq1 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq2 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq3 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq4 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq5 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq6 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq7 = kSegmentsNone;
        constexpr uint16_t kFlatTrack3x3Seq8 = kSegmentsNone;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftLargeCorkscrewDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightLargeCorkscrewDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftMediumHalfLoopDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightMediumHalfLoopDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftZeroGRollUpSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftZeroGRollUpSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftZeroGRollUpSeq2 = kSegmentsAll;
        constexpr uint16_t kRightZeroGRollUpSeq0 = kSegmentsAll;
        constexpr uint16_t kRightZeroGRollUpSeq1 = kSegmentsAll;
        constexpr uint16_t kRightZeroGRollUpSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftZeroGRollDownSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftZeroGRollDownSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftZeroGRollDownSeq2 = kSegmentsAll;
        constexpr uint16_t kRightZeroGRollDownSeq0 = kSegmentsAll;
        constexpr uint16_t kRightZeroGRollDownSeq1 = kSegmentsAll;
        constexpr uint16_t kRightZeroGRollDownSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollUpSeq3 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollUpSeq0 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollUpSeq1 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollUpSeq2 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollUpSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq0 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq1 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq2 = kSegmentsAll;
        constexpr uint16_t kLeftLargeZeroGRollDownSeq3 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollDownSeq0 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollDownSeq1 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollDownSeq2 = kSegmentsAll;
        constexpr uint16_t kRightLargeZeroGRollDownSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedDownSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedDownSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopInvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopInvertedUpSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightFlyerLargeHalfLoopUninvertedDownSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopInvertedUpSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kFlyerHalfLoopUninvertedDownSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagUp25Seq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagUp25Seq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthToDiagDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToDiagDown25Seq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthToDiagDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthToDiagDown25Seq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalUp25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalUp25Seq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthToOrthogonalDown25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthToOrthogonalDown25Seq4 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToLeftBankedUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25ToRightBankedUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToLeftBankedDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25ToRightBankedDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToRightBankedFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedFlatToLeftBankedDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedFlatToRightBankedDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToRightBankedFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedUp25ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedUp25ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToLeftBankedDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kDiagFlatToRightBankedDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagLeftBankedDown25ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagRightBankedDown25ToFlatSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25LeftBankedSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25LeftBankedSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25LeftBankedSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25LeftBankedSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagUp25RightBankedSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagUp25RightBankedSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagUp25RightBankedSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagUp25RightBankedSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25LeftBankedSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25LeftBankedSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25LeftBankedSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25LeftBankedSeq3 = kSegmentsAll;
        constexpr uint16_t kDiagDown25RightBankedSeq0 = kSegmentsAll;
        constexpr uint16_t kDiagDown25RightBankedSeq1 = kSegmentsAll;
        constexpr uint16_t kDiagDown25RightBankedSeq2 = kSegmentsAll;
        constexpr uint16_t kDiagDown25RightBankedSeq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagUp25Seq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagUp25Seq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToDiagDown25Seq4 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq0 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToDiagDown25Seq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalUp25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalUp25Seq4 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kLeftEighthBankToOrthogonalDown25Seq4 = kFlatSeq0;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq0 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq1 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq2 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq3 = kSegmentsAll;
        constexpr uint16_t kRightEighthBankToOrthogonalDown25Seq4 = kSegmentsAll;
        constexpr uint16_t kDiagBrakesSeq0 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBrakesSeq1 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBrakesSeq2 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBrakesSeq3 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq0 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq1 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq2 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBlockBrakesSeq3 = kDiagFlatSeq0;
        constexpr uint16_t kDown25BrakesSeq0 = kFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq0 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq1 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq2 = kDiagFlatSeq0;
        constexpr uint16_t kDiagBoosterSeq3 = kDiagFlatSeq0;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToUp60LongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagUp60ToFlatLongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagFlatToDown60LongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq6 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq7 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq8 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown60ToFlatLongBaseSeq9 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopUpToOrthogonalSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopUpToOrthogonalSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kLeftEighthDiveLoopDownToDiagSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq0 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq1 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq2 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq3 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq4 = kSegmentsUnimplemented;
        constexpr uint16_t kRightEighthDiveLoopDownToDiagSeq5 = kSegmentsUnimplemented;
        constexpr uint16_t kDiagDown25BrakesSeq0 = kDiagDown25Seq0;
        constexpr uint16_t kDiagDown25BrakesSeq1 = kDiagDown25Seq1;
        constexpr uint16_t kDiagDown25BrakesSeq2 = kDiagDown25Seq2;
        constexpr uint16_t kDiagDown25BrakesSeq3 = kDiagDown25Seq3;
    } // namespace Wide

    namespace SuspendedSwingingTrain
    {
        constexpr uint16_t kLeftEighthToDiagSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToDiagSeq4 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToDiagSeq0 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft);
        constexpr uint16_t kRightEighthToDiagSeq4 = EnumsToFlags(PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalSeq0 = EnumsToFlags(PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kLeftEighthToOrthogonalSeq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalSeq0 = EnumsToFlags(PaintSegment::top, PaintSegment::right, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomLeft, PaintSegment::bottomRight);
        constexpr uint16_t kRightEighthToOrthogonalSeq4 = EnumsToFlags(PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight, PaintSegment::bottomRight);
    } // namespace SuspendedSwingingTrain

    namespace WideTrain
    {
        constexpr uint16_t kSBendLeftSeq1 = kSegmentsAll;
        constexpr uint16_t kSBendLeftSeq2 = kSegmentsAll;
        constexpr uint16_t kSBendRightSeq1 = kSegmentsAll;
        constexpr uint16_t kSBendRightSeq2 = kSegmentsAll;
        constexpr uint16_t kSBendLeftCoveredSeq1 = kSBendLeftSeq1;
        constexpr uint16_t kSBendLeftCoveredSeq2 = kSBendLeftSeq2;
        constexpr uint16_t kSBendRightCoveredSeq1 = kSBendRightSeq1;
        constexpr uint16_t kSBendRightCoveredSeq2 = kSBendRightSeq2;
    } // namespace WideTrain

    static constexpr const std::array<uint16_t, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> kBlockedSegmentsNarrow[] = {
        {{ Narrow::kFlatSeq0 }}, // Flat
        {{ Narrow::kEndStationSeq0 }}, // EndStation
        {{ Narrow::kBeginStationSeq0 }}, // BeginStation
        {{ Narrow::kMiddleStationSeq0 }}, // MiddleStation
        {{ Narrow::kUp25Seq0 }}, // Up25
        {{ Narrow::kUp60Seq0 }}, // Up60
        {{ Narrow::kFlatToUp25Seq0 }}, // FlatToUp25
        {{ Narrow::kUp25ToUp60Seq0 }}, // Up25ToUp60
        {{ Narrow::kUp60ToUp25Seq0 }}, // Up60ToUp25
        {{ Narrow::kUp25ToFlatSeq0 }}, // Up25ToFlat
        {{ Narrow::kDown25Seq0 }}, // Down25
        {{ Narrow::kDown60Seq0 }}, // Down60
        {{ Narrow::kFlatToDown25Seq0 }}, // FlatToDown25
        {{ Narrow::kDown25ToDown60Seq0 }}, // Down25ToDown60
        {{ Narrow::kDown60ToDown25Seq0 }}, // Down60ToDown25
        {{ Narrow::kDown25ToFlatSeq0 }}, // Down25ToFlat
        {{ Narrow::kLeftQuarterTurn5TilesSeq0, Narrow::kLeftQuarterTurn5TilesSeq1, Narrow::kLeftQuarterTurn5TilesSeq2, Narrow::kLeftQuarterTurn5TilesSeq3, Narrow::kLeftQuarterTurn5TilesSeq4, Narrow::kLeftQuarterTurn5TilesSeq5, Narrow::kLeftQuarterTurn5TilesSeq6 }}, // LeftQuarterTurn5Tiles
        {{ Narrow::kRightQuarterTurn5TilesSeq0, Narrow::kRightQuarterTurn5TilesSeq1, Narrow::kRightQuarterTurn5TilesSeq2, Narrow::kRightQuarterTurn5TilesSeq3, Narrow::kRightQuarterTurn5TilesSeq4, Narrow::kRightQuarterTurn5TilesSeq5, Narrow::kRightQuarterTurn5TilesSeq6 }}, // RightQuarterTurn5Tiles
        {{ Narrow::kFlatToLeftBankSeq0 }}, // FlatToLeftBank
        {{ Narrow::kFlatToRightBankSeq0 }}, // FlatToRightBank
        {{ Narrow::kLeftBankToFlatSeq0 }}, // LeftBankToFlat
        {{ Narrow::kRightBankToFlatSeq0 }}, // RightBankToFlat
        {{ Narrow::kBankedLeftQuarterTurn5TilesSeq0, Narrow::kBankedLeftQuarterTurn5TilesSeq1, Narrow::kBankedLeftQuarterTurn5TilesSeq2, Narrow::kBankedLeftQuarterTurn5TilesSeq3, Narrow::kBankedLeftQuarterTurn5TilesSeq4, Narrow::kBankedLeftQuarterTurn5TilesSeq5, Narrow::kBankedLeftQuarterTurn5TilesSeq6 }}, // BankedLeftQuarterTurn5Tiles
        {{ Narrow::kBankedRightQuarterTurn5TilesSeq0, Narrow::kBankedRightQuarterTurn5TilesSeq1, Narrow::kBankedRightQuarterTurn5TilesSeq2, Narrow::kBankedRightQuarterTurn5TilesSeq3, Narrow::kBankedRightQuarterTurn5TilesSeq4, Narrow::kBankedRightQuarterTurn5TilesSeq5, Narrow::kBankedRightQuarterTurn5TilesSeq6 }}, // BankedRightQuarterTurn5Tiles
        {{ Narrow::kLeftBankToUp25Seq0 }}, // LeftBankToUp25
        {{ Narrow::kRightBankToUp25Seq0 }}, // RightBankToUp25
        {{ Narrow::kUp25ToLeftBankSeq0 }}, // Up25ToLeftBank
        {{ Narrow::kUp25ToRightBankSeq0 }}, // Up25ToRightBank
        {{ Narrow::kLeftBankToDown25Seq0 }}, // LeftBankToDown25
        {{ Narrow::kRightBankToDown25Seq0 }}, // RightBankToDown25
        {{ Narrow::kDown25ToLeftBankSeq0 }}, // Down25ToLeftBank
        {{ Narrow::kDown25ToRightBankSeq0 }}, // Down25ToRightBank
        {{ Narrow::kLeftBankSeq0 }}, // LeftBank
        {{ Narrow::kRightBankSeq0 }}, // RightBank
        {{ Narrow::kLeftQuarterTurn5TilesUp25Seq0, Narrow::kLeftQuarterTurn5TilesUp25Seq1, Narrow::kLeftQuarterTurn5TilesUp25Seq2, Narrow::kLeftQuarterTurn5TilesUp25Seq3, Narrow::kLeftQuarterTurn5TilesUp25Seq4, Narrow::kLeftQuarterTurn5TilesUp25Seq5, Narrow::kLeftQuarterTurn5TilesUp25Seq6 }}, // LeftQuarterTurn5TilesUp25
        {{ Narrow::kRightQuarterTurn5TilesUp25Seq0, Narrow::kRightQuarterTurn5TilesUp25Seq1, Narrow::kRightQuarterTurn5TilesUp25Seq2, Narrow::kRightQuarterTurn5TilesUp25Seq3, Narrow::kRightQuarterTurn5TilesUp25Seq4, Narrow::kRightQuarterTurn5TilesUp25Seq5, Narrow::kRightQuarterTurn5TilesUp25Seq6 }}, // RightQuarterTurn5TilesUp25
        {{ Narrow::kLeftQuarterTurn5TilesDown25Seq0, Narrow::kLeftQuarterTurn5TilesDown25Seq1, Narrow::kLeftQuarterTurn5TilesDown25Seq2, Narrow::kLeftQuarterTurn5TilesDown25Seq3, Narrow::kLeftQuarterTurn5TilesDown25Seq4, Narrow::kLeftQuarterTurn5TilesDown25Seq5, Narrow::kLeftQuarterTurn5TilesDown25Seq6 }}, // LeftQuarterTurn5TilesDown25
        {{ Narrow::kRightQuarterTurn5TilesDown25Seq0, Narrow::kRightQuarterTurn5TilesDown25Seq1, Narrow::kRightQuarterTurn5TilesDown25Seq2, Narrow::kRightQuarterTurn5TilesDown25Seq3, Narrow::kRightQuarterTurn5TilesDown25Seq4, Narrow::kRightQuarterTurn5TilesDown25Seq5, Narrow::kRightQuarterTurn5TilesDown25Seq6 }}, // RightQuarterTurn5TilesDown25
        {{ Narrow::kSBendLeftSeq0, Narrow::kSBendLeftSeq1, Narrow::kSBendLeftSeq2, Narrow::kSBendLeftSeq3 }}, // SBendLeft
        {{ Narrow::kSBendRightSeq0, Narrow::kSBendRightSeq1, Narrow::kSBendRightSeq2, Narrow::kSBendRightSeq3 }}, // SBendRight
        {{ Narrow::kLeftVerticalLoopSeq0, Narrow::kLeftVerticalLoopSeq1, Narrow::kLeftVerticalLoopSeq2, Narrow::kLeftVerticalLoopSeq3, Narrow::kLeftVerticalLoopSeq4, Narrow::kLeftVerticalLoopSeq5, Narrow::kLeftVerticalLoopSeq6, Narrow::kLeftVerticalLoopSeq7, Narrow::kLeftVerticalLoopSeq8, Narrow::kLeftVerticalLoopSeq9 }}, // LeftVerticalLoop
        {{ Narrow::kRightVerticalLoopSeq0, Narrow::kRightVerticalLoopSeq1, Narrow::kRightVerticalLoopSeq2, Narrow::kRightVerticalLoopSeq3, Narrow::kRightVerticalLoopSeq4, Narrow::kRightVerticalLoopSeq5, Narrow::kRightVerticalLoopSeq6, Narrow::kRightVerticalLoopSeq7, Narrow::kRightVerticalLoopSeq8, Narrow::kRightVerticalLoopSeq9 }}, // RightVerticalLoop
        {{ Narrow::kLeftQuarterTurn3TilesSeq0, Narrow::kLeftQuarterTurn3TilesSeq1, Narrow::kLeftQuarterTurn3TilesSeq2, Narrow::kLeftQuarterTurn3TilesSeq3 }}, // LeftQuarterTurn3Tiles
        {{ Narrow::kRightQuarterTurn3TilesSeq0, Narrow::kRightQuarterTurn3TilesSeq1, Narrow::kRightQuarterTurn3TilesSeq2, Narrow::kRightQuarterTurn3TilesSeq3 }}, // RightQuarterTurn3Tiles
        {{ Narrow::kLeftBankedQuarterTurn3TilesSeq0, Narrow::kLeftBankedQuarterTurn3TilesSeq1, Narrow::kLeftBankedQuarterTurn3TilesSeq2, Narrow::kLeftBankedQuarterTurn3TilesSeq3 }}, // LeftBankedQuarterTurn3Tiles
        {{ Narrow::kRightBankedQuarterTurn3TilesSeq0, Narrow::kRightBankedQuarterTurn3TilesSeq1, Narrow::kRightBankedQuarterTurn3TilesSeq2, Narrow::kRightBankedQuarterTurn3TilesSeq3 }}, // RightBankedQuarterTurn3Tiles
        {{ Narrow::kLeftQuarterTurn3TilesUp25Seq0, Narrow::kLeftQuarterTurn3TilesUp25Seq1, Narrow::kLeftQuarterTurn3TilesUp25Seq2, Narrow::kLeftQuarterTurn3TilesUp25Seq3 }}, // LeftQuarterTurn3TilesUp25
        {{ Narrow::kRightQuarterTurn3TilesUp25Seq0, Narrow::kRightQuarterTurn3TilesUp25Seq1, Narrow::kRightQuarterTurn3TilesUp25Seq2, Narrow::kRightQuarterTurn3TilesUp25Seq3 }}, // RightQuarterTurn3TilesUp25
        {{ Narrow::kLeftQuarterTurn3TilesDown25Seq0, Narrow::kLeftQuarterTurn3TilesDown25Seq1, Narrow::kLeftQuarterTurn3TilesDown25Seq2, Narrow::kLeftQuarterTurn3TilesDown25Seq3 }}, // LeftQuarterTurn3TilesDown25
        {{ Narrow::kRightQuarterTurn3TilesDown25Seq0, Narrow::kRightQuarterTurn3TilesDown25Seq1, Narrow::kRightQuarterTurn3TilesDown25Seq2, Narrow::kRightQuarterTurn3TilesDown25Seq3 }}, // RightQuarterTurn3TilesDown25
        {{ Narrow::kLeftQuarterTurn1TileSeq0 }}, // LeftQuarterTurn1Tile
        {{ Narrow::kRightQuarterTurn1TileSeq0 }}, // RightQuarterTurn1Tile
        {{ Narrow::kLeftTwistDownToUpSeq0, Narrow::kLeftTwistDownToUpSeq1, Narrow::kLeftTwistDownToUpSeq2 }}, // LeftTwistDownToUp
        {{ Narrow::kRightTwistDownToUpSeq0, Narrow::kRightTwistDownToUpSeq1, Narrow::kRightTwistDownToUpSeq2 }}, // RightTwistDownToUp
        {{ Narrow::kLeftTwistUpToDownSeq0, Narrow::kLeftTwistUpToDownSeq1, Narrow::kLeftTwistUpToDownSeq2 }}, // LeftTwistUpToDown
        {{ Narrow::kRightTwistUpToDownSeq0, Narrow::kRightTwistUpToDownSeq1, Narrow::kRightTwistUpToDownSeq2 }}, // RightTwistUpToDown
        {{ Narrow::kHalfLoopUpSeq0, Narrow::kHalfLoopUpSeq1, Narrow::kHalfLoopUpSeq2, Narrow::kHalfLoopUpSeq3 }}, // HalfLoopUp
        {{ Narrow::kHalfLoopDownSeq0, Narrow::kHalfLoopDownSeq1, Narrow::kHalfLoopDownSeq2, Narrow::kHalfLoopDownSeq3 }}, // HalfLoopDown
        {{ Narrow::kLeftCorkscrewUpSeq0, Narrow::kLeftCorkscrewUpSeq1, Narrow::kLeftCorkscrewUpSeq2 }}, // LeftCorkscrewUp
        {{ Narrow::kRightCorkscrewUpSeq0, Narrow::kRightCorkscrewUpSeq1, Narrow::kRightCorkscrewUpSeq2 }}, // RightCorkscrewUp
        {{ Narrow::kLeftCorkscrewDownSeq0, Narrow::kLeftCorkscrewDownSeq1, Narrow::kLeftCorkscrewDownSeq2 }}, // LeftCorkscrewDown
        {{ Narrow::kRightCorkscrewDownSeq0, Narrow::kRightCorkscrewDownSeq1, Narrow::kRightCorkscrewDownSeq2 }}, // RightCorkscrewDown
        {{ Narrow::kFlatToUp60Seq0 }}, // FlatToUp60
        {{ Narrow::kUp60ToFlatSeq0 }}, // Up60ToFlat
        {{ Narrow::kFlatToDown60Seq0 }}, // FlatToDown60
        {{ Narrow::kDown60ToFlatSeq0 }}, // Down60ToFlat
        {{ Narrow::kTowerBaseSeq0, Narrow::kTowerBaseSeq1, Narrow::kTowerBaseSeq2, Narrow::kTowerBaseSeq3, Narrow::kTowerBaseSeq4, Narrow::kTowerBaseSeq5, Narrow::kTowerBaseSeq6, Narrow::kTowerBaseSeq7, Narrow::kTowerBaseSeq8 }}, // TowerBase
        {{ Narrow::kTowerSectionSeq0, Narrow::kTowerSectionSeq1 }}, // TowerSection
        {{ Narrow::kFlatCoveredSeq0 }}, // FlatCovered
        {{ Narrow::kUp25CoveredSeq0 }}, // Up25Covered
        {{ Narrow::kUp60CoveredSeq0 }}, // Up60Covered
        {{ Narrow::kFlatToUp25CoveredSeq0 }}, // FlatToUp25Covered
        {{ Narrow::kUp25ToUp60CoveredSeq0 }}, // Up25ToUp60Covered
        {{ Narrow::kUp60ToUp25CoveredSeq0 }}, // Up60ToUp25Covered
        {{ Narrow::kUp25ToFlatCoveredSeq0 }}, // Up25ToFlatCovered
        {{ Narrow::kDown25CoveredSeq0 }}, // Down25Covered
        {{ Narrow::kDown60CoveredSeq0 }}, // Down60Covered
        {{ Narrow::kFlatToDown25CoveredSeq0 }}, // FlatToDown25Covered
        {{ Narrow::kDown25ToDown60CoveredSeq0 }}, // Down25ToDown60Covered
        {{ Narrow::kDown60ToDown25CoveredSeq0 }}, // Down60ToDown25Covered
        {{ Narrow::kDown25ToFlatCoveredSeq0 }}, // Down25ToFlatCovered
        {{ Narrow::kLeftQuarterTurn5TilesCoveredSeq0, Narrow::kLeftQuarterTurn5TilesCoveredSeq1, Narrow::kLeftQuarterTurn5TilesCoveredSeq2, Narrow::kLeftQuarterTurn5TilesCoveredSeq3, Narrow::kLeftQuarterTurn5TilesCoveredSeq4, Narrow::kLeftQuarterTurn5TilesCoveredSeq5, Narrow::kLeftQuarterTurn5TilesCoveredSeq6 }}, // LeftQuarterTurn5TilesCovered
        {{ Narrow::kRightQuarterTurn5TilesCoveredSeq0, Narrow::kRightQuarterTurn5TilesCoveredSeq1, Narrow::kRightQuarterTurn5TilesCoveredSeq2, Narrow::kRightQuarterTurn5TilesCoveredSeq3, Narrow::kRightQuarterTurn5TilesCoveredSeq4, Narrow::kRightQuarterTurn5TilesCoveredSeq5, Narrow::kRightQuarterTurn5TilesCoveredSeq6 }}, // RightQuarterTurn5TilesCovered
        {{ Narrow::kSBendLeftCoveredSeq0, Narrow::kSBendLeftCoveredSeq1, Narrow::kSBendLeftCoveredSeq2, Narrow::kSBendLeftCoveredSeq3 }}, // SBendLeftCovered
        {{ Narrow::kSBendRightCoveredSeq0, Narrow::kSBendRightCoveredSeq1, Narrow::kSBendRightCoveredSeq2, Narrow::kSBendRightCoveredSeq3 }}, // SBendRightCovered
        {{ Narrow::kLeftQuarterTurn3TilesCoveredSeq0, Narrow::kLeftQuarterTurn3TilesCoveredSeq1, Narrow::kLeftQuarterTurn3TilesCoveredSeq2, Narrow::kLeftQuarterTurn3TilesCoveredSeq3 }}, // LeftQuarterTurn3TilesCovered
        {{ Narrow::kRightQuarterTurn3TilesCoveredSeq0, Narrow::kRightQuarterTurn3TilesCoveredSeq1, Narrow::kRightQuarterTurn3TilesCoveredSeq2, Narrow::kRightQuarterTurn3TilesCoveredSeq3 }}, // RightQuarterTurn3TilesCovered
        {{ Narrow::kLeftHalfBankedHelixUpSmallSeq0, Narrow::kLeftHalfBankedHelixUpSmallSeq1, Narrow::kLeftHalfBankedHelixUpSmallSeq2, Narrow::kLeftHalfBankedHelixUpSmallSeq3, Narrow::kLeftHalfBankedHelixUpSmallSeq4, Narrow::kLeftHalfBankedHelixUpSmallSeq5, Narrow::kLeftHalfBankedHelixUpSmallSeq6, Narrow::kLeftHalfBankedHelixUpSmallSeq7 }}, // LeftHalfBankedHelixUpSmall
        {{ Narrow::kRightHalfBankedHelixUpSmallSeq0, Narrow::kRightHalfBankedHelixUpSmallSeq1, Narrow::kRightHalfBankedHelixUpSmallSeq2, Narrow::kRightHalfBankedHelixUpSmallSeq3, Narrow::kRightHalfBankedHelixUpSmallSeq4, Narrow::kRightHalfBankedHelixUpSmallSeq5, Narrow::kRightHalfBankedHelixUpSmallSeq6, Narrow::kRightHalfBankedHelixUpSmallSeq7 }}, // RightHalfBankedHelixUpSmall
        {{ Narrow::kLeftHalfBankedHelixDownSmallSeq0, Narrow::kLeftHalfBankedHelixDownSmallSeq1, Narrow::kLeftHalfBankedHelixDownSmallSeq2, Narrow::kLeftHalfBankedHelixDownSmallSeq3, Narrow::kLeftHalfBankedHelixDownSmallSeq4, Narrow::kLeftHalfBankedHelixDownSmallSeq5, Narrow::kLeftHalfBankedHelixDownSmallSeq6, Narrow::kLeftHalfBankedHelixDownSmallSeq7 }}, // LeftHalfBankedHelixDownSmall
        {{ Narrow::kRightHalfBankedHelixDownSmallSeq0, Narrow::kRightHalfBankedHelixDownSmallSeq1, Narrow::kRightHalfBankedHelixDownSmallSeq2, Narrow::kRightHalfBankedHelixDownSmallSeq3, Narrow::kRightHalfBankedHelixDownSmallSeq4, Narrow::kRightHalfBankedHelixDownSmallSeq5, Narrow::kRightHalfBankedHelixDownSmallSeq6, Narrow::kRightHalfBankedHelixDownSmallSeq7 }}, // RightHalfBankedHelixDownSmall
        {{ Narrow::kLeftHalfBankedHelixUpLargeSeq0, Narrow::kLeftHalfBankedHelixUpLargeSeq1, Narrow::kLeftHalfBankedHelixUpLargeSeq2, Narrow::kLeftHalfBankedHelixUpLargeSeq3, Narrow::kLeftHalfBankedHelixUpLargeSeq4, Narrow::kLeftHalfBankedHelixUpLargeSeq5, Narrow::kLeftHalfBankedHelixUpLargeSeq6, Narrow::kLeftHalfBankedHelixUpLargeSeq7, Narrow::kLeftHalfBankedHelixUpLargeSeq8, Narrow::kLeftHalfBankedHelixUpLargeSeq9, Narrow::kLeftHalfBankedHelixUpLargeSeq10, Narrow::kLeftHalfBankedHelixUpLargeSeq11, Narrow::kLeftHalfBankedHelixUpLargeSeq12, Narrow::kLeftHalfBankedHelixUpLargeSeq13 }}, // LeftHalfBankedHelixUpLarge
        {{ Narrow::kRightHalfBankedHelixUpLargeSeq0, Narrow::kRightHalfBankedHelixUpLargeSeq1, Narrow::kRightHalfBankedHelixUpLargeSeq2, Narrow::kRightHalfBankedHelixUpLargeSeq3, Narrow::kRightHalfBankedHelixUpLargeSeq4, Narrow::kRightHalfBankedHelixUpLargeSeq5, Narrow::kRightHalfBankedHelixUpLargeSeq6, Narrow::kRightHalfBankedHelixUpLargeSeq7, Narrow::kRightHalfBankedHelixUpLargeSeq8, Narrow::kRightHalfBankedHelixUpLargeSeq9, Narrow::kRightHalfBankedHelixUpLargeSeq10, Narrow::kRightHalfBankedHelixUpLargeSeq11, Narrow::kRightHalfBankedHelixUpLargeSeq12, Narrow::kRightHalfBankedHelixUpLargeSeq13 }}, // RightHalfBankedHelixUpLarge
        {{ Narrow::kLeftHalfBankedHelixDownLargeSeq0, Narrow::kLeftHalfBankedHelixDownLargeSeq1, Narrow::kLeftHalfBankedHelixDownLargeSeq2, Narrow::kLeftHalfBankedHelixDownLargeSeq3, Narrow::kLeftHalfBankedHelixDownLargeSeq4, Narrow::kLeftHalfBankedHelixDownLargeSeq5, Narrow::kLeftHalfBankedHelixDownLargeSeq6, Narrow::kLeftHalfBankedHelixDownLargeSeq7, Narrow::kLeftHalfBankedHelixDownLargeSeq8, Narrow::kLeftHalfBankedHelixDownLargeSeq9, Narrow::kLeftHalfBankedHelixDownLargeSeq10, Narrow::kLeftHalfBankedHelixDownLargeSeq11, Narrow::kLeftHalfBankedHelixDownLargeSeq12, Narrow::kLeftHalfBankedHelixDownLargeSeq13 }}, // LeftHalfBankedHelixDownLarge
        {{ Narrow::kRightHalfBankedHelixDownLargeSeq0, Narrow::kRightHalfBankedHelixDownLargeSeq1, Narrow::kRightHalfBankedHelixDownLargeSeq2, Narrow::kRightHalfBankedHelixDownLargeSeq3, Narrow::kRightHalfBankedHelixDownLargeSeq4, Narrow::kRightHalfBankedHelixDownLargeSeq5, Narrow::kRightHalfBankedHelixDownLargeSeq6, Narrow::kRightHalfBankedHelixDownLargeSeq7, Narrow::kRightHalfBankedHelixDownLargeSeq8, Narrow::kRightHalfBankedHelixDownLargeSeq9, Narrow::kRightHalfBankedHelixDownLargeSeq10, Narrow::kRightHalfBankedHelixDownLargeSeq11, Narrow::kRightHalfBankedHelixDownLargeSeq12, Narrow::kRightHalfBankedHelixDownLargeSeq13 }}, // RightHalfBankedHelixDownLarge
        {{ Narrow::kLeftQuarterTurn1TileUp60Seq0 }}, // LeftQuarterTurn1TileUp60
        {{ Narrow::kRightQuarterTurn1TileUp60Seq0 }}, // RightQuarterTurn1TileUp60
        {{ Narrow::kLeftQuarterTurn1TileDown60Seq0 }}, // LeftQuarterTurn1TileDown60
        {{ Narrow::kRightQuarterTurn1TileDown60Seq0 }}, // RightQuarterTurn1TileDown60
        {{ Narrow::kBrakesSeq0 }}, // Brakes
        {{ Narrow::kBoosterSeq0 }}, // Booster
        {{ Narrow::kMazeSeq0 }}, // Maze
        {{ Narrow::kLeftQuarterBankedHelixLargeUpSeq0, Narrow::kLeftQuarterBankedHelixLargeUpSeq1, Narrow::kLeftQuarterBankedHelixLargeUpSeq2, Narrow::kLeftQuarterBankedHelixLargeUpSeq3, Narrow::kLeftQuarterBankedHelixLargeUpSeq4, Narrow::kLeftQuarterBankedHelixLargeUpSeq5, Narrow::kLeftQuarterBankedHelixLargeUpSeq6 }}, // LeftQuarterBankedHelixLargeUp
        {{ Narrow::kRightQuarterBankedHelixLargeUpSeq0, Narrow::kRightQuarterBankedHelixLargeUpSeq1, Narrow::kRightQuarterBankedHelixLargeUpSeq2, Narrow::kRightQuarterBankedHelixLargeUpSeq3, Narrow::kRightQuarterBankedHelixLargeUpSeq4, Narrow::kRightQuarterBankedHelixLargeUpSeq5, Narrow::kRightQuarterBankedHelixLargeUpSeq6 }}, // RightQuarterBankedHelixLargeUp
        {{ Narrow::kLeftQuarterBankedHelixLargeDownSeq0, Narrow::kLeftQuarterBankedHelixLargeDownSeq1, Narrow::kLeftQuarterBankedHelixLargeDownSeq2, Narrow::kLeftQuarterBankedHelixLargeDownSeq3, Narrow::kLeftQuarterBankedHelixLargeDownSeq4, Narrow::kLeftQuarterBankedHelixLargeDownSeq5, Narrow::kLeftQuarterBankedHelixLargeDownSeq6 }}, // LeftQuarterBankedHelixLargeDown
        {{ Narrow::kRightQuarterBankedHelixLargeDownSeq0, Narrow::kRightQuarterBankedHelixLargeDownSeq1, Narrow::kRightQuarterBankedHelixLargeDownSeq2, Narrow::kRightQuarterBankedHelixLargeDownSeq3, Narrow::kRightQuarterBankedHelixLargeDownSeq4, Narrow::kRightQuarterBankedHelixLargeDownSeq5, Narrow::kRightQuarterBankedHelixLargeDownSeq6 }}, // RightQuarterBankedHelixLargeDown
        {{ Narrow::kLeftQuarterHelixLargeUpSeq0, Narrow::kLeftQuarterHelixLargeUpSeq1, Narrow::kLeftQuarterHelixLargeUpSeq2, Narrow::kLeftQuarterHelixLargeUpSeq3, Narrow::kLeftQuarterHelixLargeUpSeq4, Narrow::kLeftQuarterHelixLargeUpSeq5, Narrow::kLeftQuarterHelixLargeUpSeq6 }}, // LeftQuarterHelixLargeUp
        {{ Narrow::kRightQuarterHelixLargeUpSeq0, Narrow::kRightQuarterHelixLargeUpSeq1, Narrow::kRightQuarterHelixLargeUpSeq2, Narrow::kRightQuarterHelixLargeUpSeq3, Narrow::kRightQuarterHelixLargeUpSeq4, Narrow::kRightQuarterHelixLargeUpSeq5, Narrow::kRightQuarterHelixLargeUpSeq6 }}, // RightQuarterHelixLargeUp
        {{ Narrow::kLeftQuarterHelixLargeDownSeq0, Narrow::kLeftQuarterHelixLargeDownSeq1, Narrow::kLeftQuarterHelixLargeDownSeq2, Narrow::kLeftQuarterHelixLargeDownSeq3, Narrow::kLeftQuarterHelixLargeDownSeq4, Narrow::kLeftQuarterHelixLargeDownSeq5, Narrow::kLeftQuarterHelixLargeDownSeq6 }}, // LeftQuarterHelixLargeDown
        {{ Narrow::kRightQuarterHelixLargeDownSeq0, Narrow::kRightQuarterHelixLargeDownSeq1, Narrow::kRightQuarterHelixLargeDownSeq2, Narrow::kRightQuarterHelixLargeDownSeq3, Narrow::kRightQuarterHelixLargeDownSeq4, Narrow::kRightQuarterHelixLargeDownSeq5, Narrow::kRightQuarterHelixLargeDownSeq6 }}, // RightQuarterHelixLargeDown
        {{ Narrow::kUp25LeftBankedSeq0 }}, // Up25LeftBanked
        {{ Narrow::kUp25RightBankedSeq0 }}, // Up25RightBanked
        {{ Narrow::kWaterfallSeq0 }}, // Waterfall
        {{ Narrow::kRapidsSeq0 }}, // Rapids
        {{ Narrow::kOnRidePhotoSeq0 }}, // OnRidePhoto
        {{ Narrow::kDown25LeftBankedSeq0 }}, // Down25LeftBanked
        {{ Narrow::kDown25RightBankedSeq0 }}, // Down25RightBanked
        {{ Narrow::kWatersplashSeq0, Narrow::kWatersplashSeq1, Narrow::kWatersplashSeq2, Narrow::kWatersplashSeq3, Narrow::kWatersplashSeq4 }}, // Watersplash
        {{ Narrow::kFlatToUp60LongBaseSeq0, Narrow::kFlatToUp60LongBaseSeq1, Narrow::kFlatToUp60LongBaseSeq2, Narrow::kFlatToUp60LongBaseSeq3 }}, // FlatToUp60LongBase
        {{ Narrow::kUp60ToFlatLongBaseSeq0, Narrow::kUp60ToFlatLongBaseSeq1, Narrow::kUp60ToFlatLongBaseSeq2, Narrow::kUp60ToFlatLongBaseSeq3 }}, // Up60ToFlatLongBase
        {{ Narrow::kWhirlpoolSeq0 }}, // Whirlpool
        {{ Narrow::kDown60ToFlatLongBaseSeq0, Narrow::kDown60ToFlatLongBaseSeq1, Narrow::kDown60ToFlatLongBaseSeq2, Narrow::kDown60ToFlatLongBaseSeq3 }}, // Down60ToFlatLongBase
        {{ Narrow::kFlatToDown60LongBaseSeq0, Narrow::kFlatToDown60LongBaseSeq1, Narrow::kFlatToDown60LongBaseSeq2, Narrow::kFlatToDown60LongBaseSeq3 }}, // FlatToDown60LongBase
        {{ Narrow::kCableLiftHillSeq0, Narrow::kCableLiftHillSeq1, Narrow::kCableLiftHillSeq2, Narrow::kCableLiftHillSeq3 }}, // CableLiftHill
        {{ Narrow::kReverseFreefallSlopeSeq0, Narrow::kReverseFreefallSlopeSeq1, Narrow::kReverseFreefallSlopeSeq2, Narrow::kReverseFreefallSlopeSeq3, Narrow::kReverseFreefallSlopeSeq4, Narrow::kReverseFreefallSlopeSeq5, Narrow::kReverseFreefallSlopeSeq6 }}, // ReverseFreefallSlope
        {{ Narrow::kReverseFreefallVerticalSeq0, Narrow::kReverseFreefallVerticalSeq1 }}, // ReverseFreefallVertical
        {{ Narrow::kUp90Seq0, Narrow::kUp90Seq1 }}, // Up90
        {{ Narrow::kDown90Seq0, Narrow::kDown90Seq1 }}, // Down90
        {{ Narrow::kUp60ToUp90Seq0, Narrow::kUp60ToUp90Seq1 }}, // Up60ToUp90
        {{ Narrow::kDown90ToDown60Seq0 }}, // Down90ToDown60
        {{ Narrow::kUp90ToUp60Seq0 }}, // Up90ToUp60
        {{ Narrow::kDown60ToDown90Seq0, Narrow::kDown60ToDown90Seq1 }}, // Down60ToDown90
        {{ Narrow::kBrakeForDropSeq0 }}, // BrakeForDrop
        {{ Narrow::kLeftEighthToDiagSeq0, Narrow::kLeftEighthToDiagSeq1, Narrow::kLeftEighthToDiagSeq2, Narrow::kLeftEighthToDiagSeq3, Narrow::kLeftEighthToDiagSeq4 }}, // LeftEighthToDiag
        {{ Narrow::kRightEighthToDiagSeq0, Narrow::kRightEighthToDiagSeq1, Narrow::kRightEighthToDiagSeq2, Narrow::kRightEighthToDiagSeq3, Narrow::kRightEighthToDiagSeq4 }}, // RightEighthToDiag
        {{ Narrow::kLeftEighthToOrthogonalSeq0, Narrow::kLeftEighthToOrthogonalSeq1, Narrow::kLeftEighthToOrthogonalSeq2, Narrow::kLeftEighthToOrthogonalSeq3, Narrow::kLeftEighthToOrthogonalSeq4 }}, // LeftEighthToOrthogonal
        {{ Narrow::kRightEighthToOrthogonalSeq0, Narrow::kRightEighthToOrthogonalSeq1, Narrow::kRightEighthToOrthogonalSeq2, Narrow::kRightEighthToOrthogonalSeq3, Narrow::kRightEighthToOrthogonalSeq4 }}, // RightEighthToOrthogonal
        {{ Narrow::kLeftEighthBankToDiagSeq0, Narrow::kLeftEighthBankToDiagSeq1, Narrow::kLeftEighthBankToDiagSeq2, Narrow::kLeftEighthBankToDiagSeq3, Narrow::kLeftEighthBankToDiagSeq4 }}, // LeftEighthBankToDiag
        {{ Narrow::kRightEighthBankToDiagSeq0, Narrow::kRightEighthBankToDiagSeq1, Narrow::kRightEighthBankToDiagSeq2, Narrow::kRightEighthBankToDiagSeq3, Narrow::kRightEighthBankToDiagSeq4 }}, // RightEighthBankToDiag
        {{ Narrow::kLeftEighthBankToOrthogonalSeq0, Narrow::kLeftEighthBankToOrthogonalSeq1, Narrow::kLeftEighthBankToOrthogonalSeq2, Narrow::kLeftEighthBankToOrthogonalSeq3, Narrow::kLeftEighthBankToOrthogonalSeq4 }}, // LeftEighthBankToOrthogonal
        {{ Narrow::kRightEighthBankToOrthogonalSeq0, Narrow::kRightEighthBankToOrthogonalSeq1, Narrow::kRightEighthBankToOrthogonalSeq2, Narrow::kRightEighthBankToOrthogonalSeq3, Narrow::kRightEighthBankToOrthogonalSeq4 }}, // RightEighthBankToOrthogonal
        {{ Narrow::kDiagFlatSeq0, Narrow::kDiagFlatSeq1, Narrow::kDiagFlatSeq2, Narrow::kDiagFlatSeq3 }}, // DiagFlat
        {{ Narrow::kDiagUp25Seq0, Narrow::kDiagUp25Seq1, Narrow::kDiagUp25Seq2, Narrow::kDiagUp25Seq3 }}, // DiagUp25
        {{ Narrow::kDiagUp60Seq0, Narrow::kDiagUp60Seq1, Narrow::kDiagUp60Seq2, Narrow::kDiagUp60Seq3 }}, // DiagUp60
        {{ Narrow::kDiagFlatToUp25Seq0, Narrow::kDiagFlatToUp25Seq1, Narrow::kDiagFlatToUp25Seq2, Narrow::kDiagFlatToUp25Seq3 }}, // DiagFlatToUp25
        {{ Narrow::kDiagUp25ToUp60Seq0, Narrow::kDiagUp25ToUp60Seq1, Narrow::kDiagUp25ToUp60Seq2, Narrow::kDiagUp25ToUp60Seq3 }}, // DiagUp25ToUp60
        {{ Narrow::kDiagUp60ToUp25Seq0, Narrow::kDiagUp60ToUp25Seq1, Narrow::kDiagUp60ToUp25Seq2, Narrow::kDiagUp60ToUp25Seq3 }}, // DiagUp60ToUp25
        {{ Narrow::kDiagUp25ToFlatSeq0, Narrow::kDiagUp25ToFlatSeq1, Narrow::kDiagUp25ToFlatSeq2, Narrow::kDiagUp25ToFlatSeq3 }}, // DiagUp25ToFlat
        {{ Narrow::kDiagDown25Seq0, Narrow::kDiagDown25Seq1, Narrow::kDiagDown25Seq2, Narrow::kDiagDown25Seq3 }}, // DiagDown25
        {{ Narrow::kDiagDown60Seq0, Narrow::kDiagDown60Seq1, Narrow::kDiagDown60Seq2, Narrow::kDiagDown60Seq3 }}, // DiagDown60
        {{ Narrow::kDiagFlatToDown25Seq0, Narrow::kDiagFlatToDown25Seq1, Narrow::kDiagFlatToDown25Seq2, Narrow::kDiagFlatToDown25Seq3 }}, // DiagFlatToDown25
        {{ Narrow::kDiagDown25ToDown60Seq0, Narrow::kDiagDown25ToDown60Seq1, Narrow::kDiagDown25ToDown60Seq2, Narrow::kDiagDown25ToDown60Seq3 }}, // DiagDown25ToDown60
        {{ Narrow::kDiagDown60ToDown25Seq0, Narrow::kDiagDown60ToDown25Seq1, Narrow::kDiagDown60ToDown25Seq2, Narrow::kDiagDown60ToDown25Seq3 }}, // DiagDown60ToDown25
        {{ Narrow::kDiagDown25ToFlatSeq0, Narrow::kDiagDown25ToFlatSeq1, Narrow::kDiagDown25ToFlatSeq2, Narrow::kDiagDown25ToFlatSeq3 }}, // DiagDown25ToFlat
        {{ Narrow::kDiagFlatToUp60Seq0, Narrow::kDiagFlatToUp60Seq1, Narrow::kDiagFlatToUp60Seq2, Narrow::kDiagFlatToUp60Seq3 }}, // DiagFlatToUp60
        {{ Narrow::kDiagUp60ToFlatSeq0, Narrow::kDiagUp60ToFlatSeq1, Narrow::kDiagUp60ToFlatSeq2, Narrow::kDiagUp60ToFlatSeq3 }}, // DiagUp60ToFlat
        {{ Narrow::kDiagFlatToDown60Seq0, Narrow::kDiagFlatToDown60Seq1, Narrow::kDiagFlatToDown60Seq2, Narrow::kDiagFlatToDown60Seq3 }}, // DiagFlatToDown60
        {{ Narrow::kDiagDown60ToFlatSeq0, Narrow::kDiagDown60ToFlatSeq1, Narrow::kDiagDown60ToFlatSeq2, Narrow::kDiagDown60ToFlatSeq3 }}, // DiagDown60ToFlat
        {{ Narrow::kDiagFlatToLeftBankSeq0, Narrow::kDiagFlatToLeftBankSeq1, Narrow::kDiagFlatToLeftBankSeq2, Narrow::kDiagFlatToLeftBankSeq3 }}, // DiagFlatToLeftBank
        {{ Narrow::kDiagFlatToRightBankSeq0, Narrow::kDiagFlatToRightBankSeq1, Narrow::kDiagFlatToRightBankSeq2, Narrow::kDiagFlatToRightBankSeq3 }}, // DiagFlatToRightBank
        {{ Narrow::kDiagLeftBankToFlatSeq0, Narrow::kDiagLeftBankToFlatSeq1, Narrow::kDiagLeftBankToFlatSeq2, Narrow::kDiagLeftBankToFlatSeq3 }}, // DiagLeftBankToFlat
        {{ Narrow::kDiagRightBankToFlatSeq0, Narrow::kDiagRightBankToFlatSeq1, Narrow::kDiagRightBankToFlatSeq2, Narrow::kDiagRightBankToFlatSeq3 }}, // DiagRightBankToFlat
        {{ Narrow::kDiagLeftBankToUp25Seq0, Narrow::kDiagLeftBankToUp25Seq1, Narrow::kDiagLeftBankToUp25Seq2, Narrow::kDiagLeftBankToUp25Seq3 }}, // DiagLeftBankToUp25
        {{ Narrow::kDiagRightBankToUp25Seq0, Narrow::kDiagRightBankToUp25Seq1, Narrow::kDiagRightBankToUp25Seq2, Narrow::kDiagRightBankToUp25Seq3 }}, // DiagRightBankToUp25
        {{ Narrow::kDiagUp25ToLeftBankSeq0, Narrow::kDiagUp25ToLeftBankSeq1, Narrow::kDiagUp25ToLeftBankSeq2, Narrow::kDiagUp25ToLeftBankSeq3 }}, // DiagUp25ToLeftBank
        {{ Narrow::kDiagUp25ToRightBankSeq0, Narrow::kDiagUp25ToRightBankSeq1, Narrow::kDiagUp25ToRightBankSeq2, Narrow::kDiagUp25ToRightBankSeq3 }}, // DiagUp25ToRightBank
        {{ Narrow::kDiagLeftBankToDown25Seq0, Narrow::kDiagLeftBankToDown25Seq1, Narrow::kDiagLeftBankToDown25Seq2, Narrow::kDiagLeftBankToDown25Seq3 }}, // DiagLeftBankToDown25
        {{ Narrow::kDiagRightBankToDown25Seq0, Narrow::kDiagRightBankToDown25Seq1, Narrow::kDiagRightBankToDown25Seq2, Narrow::kDiagRightBankToDown25Seq3 }}, // DiagRightBankToDown25
        {{ Narrow::kDiagDown25ToLeftBankSeq0, Narrow::kDiagDown25ToLeftBankSeq1, Narrow::kDiagDown25ToLeftBankSeq2, Narrow::kDiagDown25ToLeftBankSeq3 }}, // DiagDown25ToLeftBank
        {{ Narrow::kDiagDown25ToRightBankSeq0, Narrow::kDiagDown25ToRightBankSeq1, Narrow::kDiagDown25ToRightBankSeq2, Narrow::kDiagDown25ToRightBankSeq3 }}, // DiagDown25ToRightBank
        {{ Narrow::kDiagLeftBankSeq0, Narrow::kDiagLeftBankSeq1, Narrow::kDiagLeftBankSeq2, Narrow::kDiagLeftBankSeq3 }}, // DiagLeftBank
        {{ Narrow::kDiagRightBankSeq0, Narrow::kDiagRightBankSeq1, Narrow::kDiagRightBankSeq2, Narrow::kDiagRightBankSeq3 }}, // DiagRightBank
        {{ Narrow::kLogFlumeReverserSeq0 }}, // LogFlumeReverser
        {{ Narrow::kSpinningTunnelSeq0 }}, // SpinningTunnel
        {{ Narrow::kLeftBarrelRollUpToDownSeq0, Narrow::kLeftBarrelRollUpToDownSeq1, Narrow::kLeftBarrelRollUpToDownSeq2 }}, // LeftBarrelRollUpToDown
        {{ Narrow::kRightBarrelRollUpToDownSeq0, Narrow::kRightBarrelRollUpToDownSeq1, Narrow::kRightBarrelRollUpToDownSeq2 }}, // RightBarrelRollUpToDown
        {{ Narrow::kLeftBarrelRollDownToUpSeq0, Narrow::kLeftBarrelRollDownToUpSeq1, Narrow::kLeftBarrelRollDownToUpSeq2 }}, // LeftBarrelRollDownToUp
        {{ Narrow::kRightBarrelRollDownToUpSeq0, Narrow::kRightBarrelRollDownToUpSeq1, Narrow::kRightBarrelRollDownToUpSeq2 }}, // RightBarrelRollDownToUp
        {{ Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq0, Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq1, Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq2, Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq3 }}, // LeftBankToLeftQuarterTurn3TilesUp25
        {{ Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq0, Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq1, Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq2, Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq3 }}, // RightBankToRightQuarterTurn3TilesUp25
        {{ Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq1, Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 }}, // LeftQuarterTurn3TilesDown25ToLeftBank
        {{ Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq0, Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq1, Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq2, Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq3 }}, // RightQuarterTurn3TilesDown25ToRightBank
        {{ Narrow::kPoweredLiftSeq0 }}, // PoweredLift
        {{ Narrow::kLeftLargeHalfLoopUpSeq0, Narrow::kLeftLargeHalfLoopUpSeq1, Narrow::kLeftLargeHalfLoopUpSeq2, Narrow::kLeftLargeHalfLoopUpSeq3, Narrow::kLeftLargeHalfLoopUpSeq4, Narrow::kLeftLargeHalfLoopUpSeq5, Narrow::kLeftLargeHalfLoopUpSeq6 }}, // LeftLargeHalfLoopUp
        {{ Narrow::kRightLargeHalfLoopUpSeq0, Narrow::kRightLargeHalfLoopUpSeq1, Narrow::kRightLargeHalfLoopUpSeq2, Narrow::kRightLargeHalfLoopUpSeq3, Narrow::kRightLargeHalfLoopUpSeq4, Narrow::kRightLargeHalfLoopUpSeq5, Narrow::kRightLargeHalfLoopUpSeq6 }}, // RightLargeHalfLoopUp
        {{ Narrow::kLeftLargeHalfLoopDownSeq0, Narrow::kLeftLargeHalfLoopDownSeq1, Narrow::kLeftLargeHalfLoopDownSeq2, Narrow::kLeftLargeHalfLoopDownSeq3, Narrow::kLeftLargeHalfLoopDownSeq4, Narrow::kLeftLargeHalfLoopDownSeq5, Narrow::kLeftLargeHalfLoopDownSeq6 }}, // LeftLargeHalfLoopDown
        {{ Narrow::kRightLargeHalfLoopDownSeq0, Narrow::kRightLargeHalfLoopDownSeq1, Narrow::kRightLargeHalfLoopDownSeq2, Narrow::kRightLargeHalfLoopDownSeq3, Narrow::kRightLargeHalfLoopDownSeq4, Narrow::kRightLargeHalfLoopDownSeq5, Narrow::kRightLargeHalfLoopDownSeq6 }}, // RightLargeHalfLoopDown
        {{ Narrow::kLeftFlyerTwistUpSeq0, Narrow::kLeftFlyerTwistUpSeq1, Narrow::kLeftFlyerTwistUpSeq2 }}, // LeftFlyerTwistUp
        {{ Narrow::kRightFlyerTwistUpSeq0, Narrow::kRightFlyerTwistUpSeq1, Narrow::kRightFlyerTwistUpSeq2 }}, // RightFlyerTwistUp
        {{ Narrow::kLeftFlyerTwistDownSeq0, Narrow::kLeftFlyerTwistDownSeq1, Narrow::kLeftFlyerTwistDownSeq2 }}, // LeftFlyerTwistDown
        {{ Narrow::kRightFlyerTwistDownSeq0, Narrow::kRightFlyerTwistDownSeq1, Narrow::kRightFlyerTwistDownSeq2 }}, // RightFlyerTwistDown
        {{ Narrow::kFlyerHalfLoopUninvertedUpSeq0, Narrow::kFlyerHalfLoopUninvertedUpSeq1, Narrow::kFlyerHalfLoopUninvertedUpSeq2, Narrow::kFlyerHalfLoopUninvertedUpSeq3 }}, // FlyerHalfLoopUninvertedUp
        {{ Narrow::kFlyerHalfLoopInvertedDownSeq0, Narrow::kFlyerHalfLoopInvertedDownSeq1, Narrow::kFlyerHalfLoopInvertedDownSeq2, Narrow::kFlyerHalfLoopInvertedDownSeq3 }}, // FlyerHalfLoopInvertedDown
        {{ Narrow::kLeftFlyerCorkscrewUpSeq0, Narrow::kLeftFlyerCorkscrewUpSeq1, Narrow::kLeftFlyerCorkscrewUpSeq2 }}, // LeftFlyerCorkscrewUp
        {{ Narrow::kRightFlyerCorkscrewUpSeq0, Narrow::kRightFlyerCorkscrewUpSeq1, Narrow::kRightFlyerCorkscrewUpSeq2 }}, // RightFlyerCorkscrewUp
        {{ Narrow::kLeftFlyerCorkscrewDownSeq0, Narrow::kLeftFlyerCorkscrewDownSeq1, Narrow::kLeftFlyerCorkscrewDownSeq2 }}, // LeftFlyerCorkscrewDown
        {{ Narrow::kRightFlyerCorkscrewDownSeq0, Narrow::kRightFlyerCorkscrewDownSeq1, Narrow::kRightFlyerCorkscrewDownSeq2 }}, // RightFlyerCorkscrewDown
        {{ Narrow::kHeartLineTransferUpSeq0, Narrow::kHeartLineTransferUpSeq1, Narrow::kHeartLineTransferUpSeq2, Narrow::kHeartLineTransferUpSeq3 }}, // HeartLineTransferUp
        {{ Narrow::kHeartLineTransferDownSeq0, Narrow::kHeartLineTransferDownSeq1, Narrow::kHeartLineTransferDownSeq2, Narrow::kHeartLineTransferDownSeq3 }}, // HeartLineTransferDown
        {{ Narrow::kLeftHeartLineRollSeq0, Narrow::kLeftHeartLineRollSeq1, Narrow::kLeftHeartLineRollSeq2, Narrow::kLeftHeartLineRollSeq3, Narrow::kLeftHeartLineRollSeq4, Narrow::kLeftHeartLineRollSeq5 }}, // LeftHeartLineRoll
        {{ Narrow::kRightHeartLineRollSeq0, Narrow::kRightHeartLineRollSeq1, Narrow::kRightHeartLineRollSeq2, Narrow::kRightHeartLineRollSeq3, Narrow::kRightHeartLineRollSeq4, Narrow::kRightHeartLineRollSeq5 }}, // RightHeartLineRoll
        {{ Narrow::kMinigolfHoleASeq0, Narrow::kMinigolfHoleASeq1 }}, // MinigolfHoleA
        {{ Narrow::kMinigolfHoleBSeq0, Narrow::kMinigolfHoleBSeq1 }}, // MinigolfHoleB
        {{ Narrow::kMinigolfHoleCSeq0, Narrow::kMinigolfHoleCSeq1 }}, // MinigolfHoleC
        {{ Narrow::kMinigolfHoleDSeq0, Narrow::kMinigolfHoleDSeq1, Narrow::kMinigolfHoleDSeq2 }}, // MinigolfHoleD
        {{ Narrow::kMinigolfHoleESeq0, Narrow::kMinigolfHoleESeq1, Narrow::kMinigolfHoleESeq2 }}, // MinigolfHoleE
        {{ Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq0, Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq1, Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq2, Narrow::kMultiDimInvertedFlatToDown90QuarterLoopSeq3 }}, // MultiDimInvertedFlatToDown90QuarterLoop
        {{ Narrow::kUp90ToInvertedFlatQuarterLoopSeq0, Narrow::kUp90ToInvertedFlatQuarterLoopSeq1, Narrow::kUp90ToInvertedFlatQuarterLoopSeq2 }}, // Up90ToInvertedFlatQuarterLoop
        {{ Narrow::kInvertedFlatToDown90QuarterLoopSeq0, Narrow::kInvertedFlatToDown90QuarterLoopSeq1, Narrow::kInvertedFlatToDown90QuarterLoopSeq2, Narrow::kInvertedFlatToDown90QuarterLoopSeq3 }}, // InvertedFlatToDown90QuarterLoop
        {{ Narrow::kLeftCurvedLiftHillSeq0, Narrow::kLeftCurvedLiftHillSeq1, Narrow::kLeftCurvedLiftHillSeq2, Narrow::kLeftCurvedLiftHillSeq3 }}, // LeftCurvedLiftHill
        {{ Narrow::kRightCurvedLiftHillSeq0, Narrow::kRightCurvedLiftHillSeq1, Narrow::kRightCurvedLiftHillSeq2, Narrow::kRightCurvedLiftHillSeq3 }}, // RightCurvedLiftHill
        {{ Narrow::kLeftReverserSeq0, Narrow::kLeftReverserSeq1, Narrow::kLeftReverserSeq2, Narrow::kLeftReverserSeq3, Narrow::kLeftReverserSeq4, Narrow::kLeftReverserSeq5 }}, // LeftReverser
        {{ Narrow::kRightReverserSeq0, Narrow::kRightReverserSeq1, Narrow::kRightReverserSeq2, Narrow::kRightReverserSeq3, Narrow::kRightReverserSeq4, Narrow::kRightReverserSeq5 }}, // RightReverser
        {{ Narrow::kAirThrustTopCapSeq0, Narrow::kAirThrustTopCapSeq1, Narrow::kAirThrustTopCapSeq2, Narrow::kAirThrustTopCapSeq3 }}, // AirThrustTopCap
        {{ Narrow::kAirThrustVerticalDownSeq0, Narrow::kAirThrustVerticalDownSeq1 }}, // AirThrustVerticalDown
        {{ Narrow::kAirThrustVerticalDownToLevelSeq0, Narrow::kAirThrustVerticalDownToLevelSeq1, Narrow::kAirThrustVerticalDownToLevelSeq2, Narrow::kAirThrustVerticalDownToLevelSeq3, Narrow::kAirThrustVerticalDownToLevelSeq4, Narrow::kAirThrustVerticalDownToLevelSeq5, Narrow::kAirThrustVerticalDownToLevelSeq6 }}, // AirThrustVerticalDownToLevel
        {{ Narrow::kBlockBrakesSeq0 }}, // BlockBrakes
        {{ Narrow::kLeftBankedQuarterTurn3TileUp25Seq0, Narrow::kLeftBankedQuarterTurn3TileUp25Seq1, Narrow::kLeftBankedQuarterTurn3TileUp25Seq2, Narrow::kLeftBankedQuarterTurn3TileUp25Seq3 }}, // LeftBankedQuarterTurn3TileUp25
        {{ Narrow::kRightBankedQuarterTurn3TileUp25Seq0, Narrow::kRightBankedQuarterTurn3TileUp25Seq1, Narrow::kRightBankedQuarterTurn3TileUp25Seq2, Narrow::kRightBankedQuarterTurn3TileUp25Seq3 }}, // RightBankedQuarterTurn3TileUp25
        {{ Narrow::kLeftBankedQuarterTurn3TileDown25Seq0, Narrow::kLeftBankedQuarterTurn3TileDown25Seq1, Narrow::kLeftBankedQuarterTurn3TileDown25Seq2, Narrow::kLeftBankedQuarterTurn3TileDown25Seq3 }}, // LeftBankedQuarterTurn3TileDown25
        {{ Narrow::kRightBankedQuarterTurn3TileDown25Seq0, Narrow::kRightBankedQuarterTurn3TileDown25Seq1, Narrow::kRightBankedQuarterTurn3TileDown25Seq2, Narrow::kRightBankedQuarterTurn3TileDown25Seq3 }}, // RightBankedQuarterTurn3TileDown25
        {{ Narrow::kLeftBankedQuarterTurn5TileUp25Seq0, Narrow::kLeftBankedQuarterTurn5TileUp25Seq1, Narrow::kLeftBankedQuarterTurn5TileUp25Seq2, Narrow::kLeftBankedQuarterTurn5TileUp25Seq3, Narrow::kLeftBankedQuarterTurn5TileUp25Seq4, Narrow::kLeftBankedQuarterTurn5TileUp25Seq5, Narrow::kLeftBankedQuarterTurn5TileUp25Seq6 }}, // LeftBankedQuarterTurn5TileUp25
        {{ Narrow::kRightBankedQuarterTurn5TileUp25Seq0, Narrow::kRightBankedQuarterTurn5TileUp25Seq1, Narrow::kRightBankedQuarterTurn5TileUp25Seq2, Narrow::kRightBankedQuarterTurn5TileUp25Seq3, Narrow::kRightBankedQuarterTurn5TileUp25Seq4, Narrow::kRightBankedQuarterTurn5TileUp25Seq5, Narrow::kRightBankedQuarterTurn5TileUp25Seq6 }}, // RightBankedQuarterTurn5TileUp25
        {{ Narrow::kLeftBankedQuarterTurn5TileDown25Seq0, Narrow::kLeftBankedQuarterTurn5TileDown25Seq1, Narrow::kLeftBankedQuarterTurn5TileDown25Seq2, Narrow::kLeftBankedQuarterTurn5TileDown25Seq3, Narrow::kLeftBankedQuarterTurn5TileDown25Seq4, Narrow::kLeftBankedQuarterTurn5TileDown25Seq5, Narrow::kLeftBankedQuarterTurn5TileDown25Seq6 }}, // LeftBankedQuarterTurn5TileDown25
        {{ Narrow::kRightBankedQuarterTurn5TileDown25Seq0, Narrow::kRightBankedQuarterTurn5TileDown25Seq1, Narrow::kRightBankedQuarterTurn5TileDown25Seq2, Narrow::kRightBankedQuarterTurn5TileDown25Seq3, Narrow::kRightBankedQuarterTurn5TileDown25Seq4, Narrow::kRightBankedQuarterTurn5TileDown25Seq5, Narrow::kRightBankedQuarterTurn5TileDown25Seq6 }}, // RightBankedQuarterTurn5TileDown25
        {{ Narrow::kUp25ToLeftBankedUp25Seq0 }}, // Up25ToLeftBankedUp25
        {{ Narrow::kUp25ToRightBankedUp25Seq0 }}, // Up25ToRightBankedUp25
        {{ Narrow::kLeftBankedUp25ToUp25Seq0 }}, // LeftBankedUp25ToUp25
        {{ Narrow::kRightBankedUp25ToUp25Seq0 }}, // RightBankedUp25ToUp25
        {{ Narrow::kDown25ToLeftBankedDown25Seq0 }}, // Down25ToLeftBankedDown25
        {{ Narrow::kDown25ToRightBankedDown25Seq0 }}, // Down25ToRightBankedDown25
        {{ Narrow::kLeftBankedDown25ToDown25Seq0 }}, // LeftBankedDown25ToDown25
        {{ Narrow::kRightBankedDown25ToDown25Seq0 }}, // RightBankedDown25ToDown25
        {{ Narrow::kLeftBankedFlatToLeftBankedUp25Seq0 }}, // LeftBankedFlatToLeftBankedUp25
        {{ Narrow::kRightBankedFlatToRightBankedUp25Seq0 }}, // RightBankedFlatToRightBankedUp25
        {{ Narrow::kLeftBankedUp25ToLeftBankedFlatSeq0 }}, // LeftBankedUp25ToLeftBankedFlat
        {{ Narrow::kRightBankedUp25ToRightBankedFlatSeq0 }}, // RightBankedUp25ToRightBankedFlat
        {{ Narrow::kLeftBankedFlatToLeftBankedDown25Seq0 }}, // LeftBankedFlatToLeftBankedDown25
        {{ Narrow::kRightBankedFlatToRightBankedDown25Seq0 }}, // RightBankedFlatToRightBankedDown25
        {{ Narrow::kLeftBankedDown25ToLeftBankedFlatSeq0 }}, // LeftBankedDown25ToLeftBankedFlat
        {{ Narrow::kRightBankedDown25ToRightBankedFlatSeq0 }}, // RightBankedDown25ToRightBankedFlat
        {{ Narrow::kFlatToLeftBankedUp25Seq0 }}, // FlatToLeftBankedUp25
        {{ Narrow::kFlatToRightBankedUp25Seq0 }}, // FlatToRightBankedUp25
        {{ Narrow::kLeftBankedUp25ToFlatSeq0 }}, // LeftBankedUp25ToFlat
        {{ Narrow::kRightBankedUp25ToFlatSeq0 }}, // RightBankedUp25ToFlat
        {{ Narrow::kFlatToLeftBankedDown25Seq0 }}, // FlatToLeftBankedDown25
        {{ Narrow::kFlatToRightBankedDown25Seq0 }}, // FlatToRightBankedDown25
        {{ Narrow::kLeftBankedDown25ToFlatSeq0 }}, // LeftBankedDown25ToFlat
        {{ Narrow::kRightBankedDown25ToFlatSeq0 }}, // RightBankedDown25ToFlat
        {{ Narrow::kLeftQuarterTurn1TileUp90Seq0, Narrow::kLeftQuarterTurn1TileUp90Seq1 }}, // LeftQuarterTurn1TileUp90
        {{ Narrow::kRightQuarterTurn1TileUp90Seq0, Narrow::kRightQuarterTurn1TileUp90Seq1 }}, // RightQuarterTurn1TileUp90
        {{ Narrow::kLeftQuarterTurn1TileDown90Seq0, Narrow::kLeftQuarterTurn1TileDown90Seq1 }}, // LeftQuarterTurn1TileDown90
        {{ Narrow::kRightQuarterTurn1TileDown90Seq0, Narrow::kRightQuarterTurn1TileDown90Seq1 }}, // RightQuarterTurn1TileDown90
        {{ Narrow::kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, Narrow::kMultiDimUp90ToInvertedFlatQuarterLoopSeq1, Narrow::kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 }}, // MultiDimUp90ToInvertedFlatQuarterLoop
        {{ Narrow::kMultiDimFlatToDown90QuarterLoopSeq0, Narrow::kMultiDimFlatToDown90QuarterLoopSeq1, Narrow::kMultiDimFlatToDown90QuarterLoopSeq2, Narrow::kMultiDimFlatToDown90QuarterLoopSeq3 }}, // MultiDimFlatToDown90QuarterLoop
        {{ Narrow::kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, Narrow::kMultiDimInvertedUp90ToFlatQuarterLoopSeq1, Narrow::kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 }}, // MultiDimInvertedUp90ToFlatQuarterLoop
        {{ Narrow::kRotationControlToggleSeq0 }}, // RotationControlToggle
        {{ Narrow::kFlatTrack1x4ASeq0, Narrow::kFlatTrack1x4ASeq1, Narrow::kFlatTrack1x4ASeq2, Narrow::kFlatTrack1x4ASeq3 }}, // FlatTrack1x4A
        {{ Narrow::kFlatTrack2x2Seq0, Narrow::kFlatTrack2x2Seq1, Narrow::kFlatTrack2x2Seq2, Narrow::kFlatTrack2x2Seq3 }}, // FlatTrack2x2
        {{ Narrow::kFlatTrack4x4Seq0, Narrow::kFlatTrack4x4Seq1, Narrow::kFlatTrack4x4Seq2, Narrow::kFlatTrack4x4Seq3, Narrow::kFlatTrack4x4Seq4, Narrow::kFlatTrack4x4Seq5, Narrow::kFlatTrack4x4Seq6, Narrow::kFlatTrack4x4Seq7, Narrow::kFlatTrack4x4Seq8, Narrow::kFlatTrack4x4Seq9, Narrow::kFlatTrack4x4Seq10, Narrow::kFlatTrack4x4Seq11, Narrow::kFlatTrack4x4Seq12, Narrow::kFlatTrack4x4Seq13, Narrow::kFlatTrack4x4Seq14, Narrow::kFlatTrack4x4Seq15 }}, // FlatTrack4x4
        {{ Narrow::kFlatTrack2x4Seq0, Narrow::kFlatTrack2x4Seq1, Narrow::kFlatTrack2x4Seq2, Narrow::kFlatTrack2x4Seq3, Narrow::kFlatTrack2x4Seq4, Narrow::kFlatTrack2x4Seq5, Narrow::kFlatTrack2x4Seq6, Narrow::kFlatTrack2x4Seq7 }}, // FlatTrack2x4
        {{ Narrow::kFlatTrack1x5Seq0, Narrow::kFlatTrack1x5Seq1, Narrow::kFlatTrack1x5Seq2, Narrow::kFlatTrack1x5Seq3, Narrow::kFlatTrack1x5Seq4 }}, // FlatTrack1x5
        {{ Narrow::kFlatTrack1x1ASeq0 }}, // FlatTrack1x1A
        {{ Narrow::kFlatTrack1x4BSeq0, Narrow::kFlatTrack1x4BSeq1, Narrow::kFlatTrack1x4BSeq2, Narrow::kFlatTrack1x4BSeq3 }}, // FlatTrack1x4B
        {{ Narrow::kFlatTrack1x1BSeq0 }}, // FlatTrack1x1B
        {{ Narrow::kFlatTrack1x4CSeq0, Narrow::kFlatTrack1x4CSeq1, Narrow::kFlatTrack1x4CSeq2, Narrow::kFlatTrack1x4CSeq3 }}, // FlatTrack1x4C
        {{ Narrow::kFlatTrack3x3Seq0, Narrow::kFlatTrack3x3Seq1, Narrow::kFlatTrack3x3Seq2, Narrow::kFlatTrack3x3Seq3, Narrow::kFlatTrack3x3Seq4, Narrow::kFlatTrack3x3Seq5, Narrow::kFlatTrack3x3Seq6, Narrow::kFlatTrack3x3Seq7, Narrow::kFlatTrack3x3Seq8 }}, // FlatTrack3x3
        {{ Narrow::kLeftLargeCorkscrewUpSeq0, Narrow::kLeftLargeCorkscrewUpSeq1, Narrow::kLeftLargeCorkscrewUpSeq2, Narrow::kLeftLargeCorkscrewUpSeq3, Narrow::kLeftLargeCorkscrewUpSeq4, Narrow::kLeftLargeCorkscrewUpSeq5 }}, // LeftLargeCorkscrewUp
        {{ Narrow::kRightLargeCorkscrewUpSeq0, Narrow::kRightLargeCorkscrewUpSeq1, Narrow::kRightLargeCorkscrewUpSeq2, Narrow::kRightLargeCorkscrewUpSeq3, Narrow::kRightLargeCorkscrewUpSeq4, Narrow::kRightLargeCorkscrewUpSeq5 }}, // RightLargeCorkscrewUp
        {{ Narrow::kLeftLargeCorkscrewDownSeq0, Narrow::kLeftLargeCorkscrewDownSeq1, Narrow::kLeftLargeCorkscrewDownSeq2, Narrow::kLeftLargeCorkscrewDownSeq3, Narrow::kLeftLargeCorkscrewDownSeq4, Narrow::kLeftLargeCorkscrewDownSeq5 }}, // LeftLargeCorkscrewDown
        {{ Narrow::kRightLargeCorkscrewDownSeq0, Narrow::kRightLargeCorkscrewDownSeq1, Narrow::kRightLargeCorkscrewDownSeq2, Narrow::kRightLargeCorkscrewDownSeq3, Narrow::kRightLargeCorkscrewDownSeq4, Narrow::kRightLargeCorkscrewDownSeq5 }}, // RightLargeCorkscrewDown
        {{ Narrow::kLeftMediumHalfLoopUpSeq0, Narrow::kLeftMediumHalfLoopUpSeq1, Narrow::kLeftMediumHalfLoopUpSeq2, Narrow::kLeftMediumHalfLoopUpSeq3, Narrow::kLeftMediumHalfLoopUpSeq4 }}, // LeftMediumHalfLoopUp
        {{ Narrow::kRightMediumHalfLoopUpSeq0, Narrow::kRightMediumHalfLoopUpSeq1, Narrow::kRightMediumHalfLoopUpSeq2, Narrow::kRightMediumHalfLoopUpSeq3, Narrow::kRightMediumHalfLoopUpSeq4 }}, // RightMediumHalfLoopUp
        {{ Narrow::kLeftMediumHalfLoopDownSeq0, Narrow::kLeftMediumHalfLoopDownSeq1, Narrow::kLeftMediumHalfLoopDownSeq2, Narrow::kLeftMediumHalfLoopDownSeq3, Narrow::kLeftMediumHalfLoopDownSeq4 }}, // LeftMediumHalfLoopDown
        {{ Narrow::kRightMediumHalfLoopDownSeq0, Narrow::kRightMediumHalfLoopDownSeq1, Narrow::kRightMediumHalfLoopDownSeq2, Narrow::kRightMediumHalfLoopDownSeq3, Narrow::kRightMediumHalfLoopDownSeq4 }}, // RightMediumHalfLoopDown
        {{ Narrow::kLeftZeroGRollUpSeq0, Narrow::kLeftZeroGRollUpSeq1, Narrow::kLeftZeroGRollUpSeq2 }}, // LeftZeroGRollUp
        {{ Narrow::kRightZeroGRollUpSeq0, Narrow::kRightZeroGRollUpSeq1, Narrow::kRightZeroGRollUpSeq2 }}, // RightZeroGRollUp
        {{ Narrow::kLeftZeroGRollDownSeq0, Narrow::kLeftZeroGRollDownSeq1, Narrow::kLeftZeroGRollDownSeq2 }}, // LeftZeroGRollDown
        {{ Narrow::kRightZeroGRollDownSeq0, Narrow::kRightZeroGRollDownSeq1, Narrow::kRightZeroGRollDownSeq2 }}, // RightZeroGRollDown
        {{ Narrow::kLeftLargeZeroGRollUpSeq0, Narrow::kLeftLargeZeroGRollUpSeq1, Narrow::kLeftLargeZeroGRollUpSeq2, Narrow::kLeftLargeZeroGRollUpSeq3 }}, // LeftLargeZeroGRollUp
        {{ Narrow::kRightLargeZeroGRollUpSeq0, Narrow::kRightLargeZeroGRollUpSeq1, Narrow::kRightLargeZeroGRollUpSeq2, Narrow::kRightLargeZeroGRollUpSeq3 }}, // RightLargeZeroGRollUp
        {{ Narrow::kLeftLargeZeroGRollDownSeq0, Narrow::kLeftLargeZeroGRollDownSeq1, Narrow::kLeftLargeZeroGRollDownSeq2, Narrow::kLeftLargeZeroGRollDownSeq3 }}, // LeftLargeZeroGRollDown
        {{ Narrow::kRightLargeZeroGRollDownSeq0, Narrow::kRightLargeZeroGRollDownSeq1, Narrow::kRightLargeZeroGRollDownSeq2, Narrow::kRightLargeZeroGRollDownSeq3 }}, // RightLargeZeroGRollDown
        {{ Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq0, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq1, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq2, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq3, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq4, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq5, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedUp
        {{ Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq0, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq1, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq2, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq3, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq4, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq5, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq6 }}, // RightFlyerLargeHalfLoopUninvertedUp
        {{ Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq0, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq1, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq2, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq3, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq4, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq5, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopInvertedDown
        {{ Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq0, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq1, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq2, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq3, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq4, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq5, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq6 }}, // RightFlyerLargeHalfLoopInvertedDown
        {{ Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq0, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq1, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq2, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq3, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq4, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq5, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopInvertedUp
        {{ Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq0, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq1, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq2, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq3, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq4, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq5, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq6 }}, // RightFlyerLargeHalfLoopInvertedUp
        {{ Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq0, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq1, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq2, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq3, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq4, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq5, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedDown
        {{ Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq0, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq1, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq2, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq3, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq4, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq5, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq6 }}, // RightFlyerLargeHalfLoopUninvertedDown
        {{ Narrow::kFlyerHalfLoopInvertedUpSeq0, Narrow::kFlyerHalfLoopInvertedUpSeq1, Narrow::kFlyerHalfLoopInvertedUpSeq2, Narrow::kFlyerHalfLoopInvertedUpSeq3 }}, // FlyerHalfLoopInvertedUp
        {{ Narrow::kFlyerHalfLoopUninvertedDownSeq0, Narrow::kFlyerHalfLoopUninvertedDownSeq1, Narrow::kFlyerHalfLoopUninvertedDownSeq2, Narrow::kFlyerHalfLoopUninvertedDownSeq3 }}, // FlyerHalfLoopUninvertedDown
        {{ Narrow::kLeftEighthToDiagUp25Seq0, Narrow::kLeftEighthToDiagUp25Seq1, Narrow::kLeftEighthToDiagUp25Seq2, Narrow::kLeftEighthToDiagUp25Seq3, Narrow::kLeftEighthToDiagUp25Seq4 }}, // LeftEighthToDiagUp25
        {{ Narrow::kRightEighthToDiagUp25Seq0, Narrow::kRightEighthToDiagUp25Seq1, Narrow::kRightEighthToDiagUp25Seq2, Narrow::kRightEighthToDiagUp25Seq3, Narrow::kRightEighthToDiagUp25Seq4 }}, // RightEighthToDiagUp25
        {{ Narrow::kLeftEighthToDiagDown25Seq0, Narrow::kLeftEighthToDiagDown25Seq1, Narrow::kLeftEighthToDiagDown25Seq2, Narrow::kLeftEighthToDiagDown25Seq3, Narrow::kLeftEighthToDiagDown25Seq4 }}, // LeftEighthToDiagDown25
        {{ Narrow::kRightEighthToDiagDown25Seq0, Narrow::kRightEighthToDiagDown25Seq1, Narrow::kRightEighthToDiagDown25Seq2, Narrow::kRightEighthToDiagDown25Seq3, Narrow::kRightEighthToDiagDown25Seq4 }}, // RightEighthToDiagDown25
        {{ Narrow::kLeftEighthToOrthogonalUp25Seq0, Narrow::kLeftEighthToOrthogonalUp25Seq1, Narrow::kLeftEighthToOrthogonalUp25Seq2, Narrow::kLeftEighthToOrthogonalUp25Seq3, Narrow::kLeftEighthToOrthogonalUp25Seq4 }}, // LeftEighthToOrthogonalUp25
        {{ Narrow::kRightEighthToOrthogonalUp25Seq0, Narrow::kRightEighthToOrthogonalUp25Seq1, Narrow::kRightEighthToOrthogonalUp25Seq2, Narrow::kRightEighthToOrthogonalUp25Seq3, Narrow::kRightEighthToOrthogonalUp25Seq4 }}, // RightEighthToOrthogonalUp25
        {{ Narrow::kLeftEighthToOrthogonalDown25Seq0, Narrow::kLeftEighthToOrthogonalDown25Seq1, Narrow::kLeftEighthToOrthogonalDown25Seq2, Narrow::kLeftEighthToOrthogonalDown25Seq3, Narrow::kLeftEighthToOrthogonalDown25Seq4 }}, // LeftEighthToOrthogonalDown25
        {{ Narrow::kRightEighthToOrthogonalDown25Seq0, Narrow::kRightEighthToOrthogonalDown25Seq1, Narrow::kRightEighthToOrthogonalDown25Seq2, Narrow::kRightEighthToOrthogonalDown25Seq3, Narrow::kRightEighthToOrthogonalDown25Seq4 }}, // RightEighthToOrthogonalDown25
        {{ Narrow::kDiagUp25ToLeftBankedUp25Seq0, Narrow::kDiagUp25ToLeftBankedUp25Seq1, Narrow::kDiagUp25ToLeftBankedUp25Seq2, Narrow::kDiagUp25ToLeftBankedUp25Seq3 }}, // DiagUp25ToLeftBankedUp25
        {{ Narrow::kDiagUp25ToRightBankedUp25Seq0, Narrow::kDiagUp25ToRightBankedUp25Seq1, Narrow::kDiagUp25ToRightBankedUp25Seq2, Narrow::kDiagUp25ToRightBankedUp25Seq3 }}, // DiagUp25ToRightBankedUp25
        {{ Narrow::kDiagLeftBankedUp25ToUp25Seq0, Narrow::kDiagLeftBankedUp25ToUp25Seq1, Narrow::kDiagLeftBankedUp25ToUp25Seq2, Narrow::kDiagLeftBankedUp25ToUp25Seq3 }}, // DiagLeftBankedUp25ToUp25
        {{ Narrow::kDiagRightBankedUp25ToUp25Seq0, Narrow::kDiagRightBankedUp25ToUp25Seq1, Narrow::kDiagRightBankedUp25ToUp25Seq2, Narrow::kDiagRightBankedUp25ToUp25Seq3 }}, // DiagRightBankedUp25ToUp25
        {{ Narrow::kDiagDown25ToLeftBankedDown25Seq0, Narrow::kDiagDown25ToLeftBankedDown25Seq1, Narrow::kDiagDown25ToLeftBankedDown25Seq2, Narrow::kDiagDown25ToLeftBankedDown25Seq3 }}, // DiagDown25ToLeftBankedDown25
        {{ Narrow::kDiagDown25ToRightBankedDown25Seq0, Narrow::kDiagDown25ToRightBankedDown25Seq1, Narrow::kDiagDown25ToRightBankedDown25Seq2, Narrow::kDiagDown25ToRightBankedDown25Seq3 }}, // DiagDown25ToRightBankedDown25
        {{ Narrow::kDiagLeftBankedDown25ToDown25Seq0, Narrow::kDiagLeftBankedDown25ToDown25Seq1, Narrow::kDiagLeftBankedDown25ToDown25Seq2, Narrow::kDiagLeftBankedDown25ToDown25Seq3 }}, // DiagLeftBankedDown25ToDown25
        {{ Narrow::kDiagRightBankedDown25ToDown25Seq0, Narrow::kDiagRightBankedDown25ToDown25Seq1, Narrow::kDiagRightBankedDown25ToDown25Seq2, Narrow::kDiagRightBankedDown25ToDown25Seq3 }}, // DiagRightBankedDown25ToDown25
        {{ Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq0, Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq1, Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq2, Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq3 }}, // DiagLeftBankedFlatToLeftBankedUp25
        {{ Narrow::kDiagRightBankedFlatToRightBankedUp25Seq0, Narrow::kDiagRightBankedFlatToRightBankedUp25Seq1, Narrow::kDiagRightBankedFlatToRightBankedUp25Seq2, Narrow::kDiagRightBankedFlatToRightBankedUp25Seq3 }}, // DiagRightBankedFlatToRightBankedUp25
        {{ Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq0, Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq1, Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq2, Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedUp25ToLeftBankedFlat
        {{ Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq0, Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq1, Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq2, Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq3 }}, // DiagRightBankedUp25ToRightBankedFlat
        {{ Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq0, Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq1, Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq2, Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq3 }}, // DiagLeftBankedFlatToLeftBankedDown25
        {{ Narrow::kDiagRightBankedFlatToRightBankedDown25Seq0, Narrow::kDiagRightBankedFlatToRightBankedDown25Seq1, Narrow::kDiagRightBankedFlatToRightBankedDown25Seq2, Narrow::kDiagRightBankedFlatToRightBankedDown25Seq3 }}, // DiagRightBankedFlatToRightBankedDown25
        {{ Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq0, Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq1, Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq2, Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedDown25ToLeftBankedFlat
        {{ Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq0, Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq1, Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq2, Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq3 }}, // DiagRightBankedDown25ToRightBankedFlat
        {{ Narrow::kDiagFlatToLeftBankedUp25Seq0, Narrow::kDiagFlatToLeftBankedUp25Seq1, Narrow::kDiagFlatToLeftBankedUp25Seq2, Narrow::kDiagFlatToLeftBankedUp25Seq3 }}, // DiagFlatToLeftBankedUp25
        {{ Narrow::kDiagFlatToRightBankedUp25Seq0, Narrow::kDiagFlatToRightBankedUp25Seq1, Narrow::kDiagFlatToRightBankedUp25Seq2, Narrow::kDiagFlatToRightBankedUp25Seq3 }}, // DiagFlatToRightBankedUp25
        {{ Narrow::kDiagLeftBankedUp25ToFlatSeq0, Narrow::kDiagLeftBankedUp25ToFlatSeq1, Narrow::kDiagLeftBankedUp25ToFlatSeq2, Narrow::kDiagLeftBankedUp25ToFlatSeq3 }}, // DiagLeftBankedUp25ToFlat
        {{ Narrow::kDiagRightBankedUp25ToFlatSeq0, Narrow::kDiagRightBankedUp25ToFlatSeq1, Narrow::kDiagRightBankedUp25ToFlatSeq2, Narrow::kDiagRightBankedUp25ToFlatSeq3 }}, // DiagRightBankedUp25ToFlat
        {{ Narrow::kDiagFlatToLeftBankedDown25Seq0, Narrow::kDiagFlatToLeftBankedDown25Seq1, Narrow::kDiagFlatToLeftBankedDown25Seq2, Narrow::kDiagFlatToLeftBankedDown25Seq3 }}, // DiagFlatToLeftBankedDown25
        {{ Narrow::kDiagFlatToRightBankedDown25Seq0, Narrow::kDiagFlatToRightBankedDown25Seq1, Narrow::kDiagFlatToRightBankedDown25Seq2, Narrow::kDiagFlatToRightBankedDown25Seq3 }}, // DiagFlatToRightBankedDown25
        {{ Narrow::kDiagLeftBankedDown25ToFlatSeq0, Narrow::kDiagLeftBankedDown25ToFlatSeq1, Narrow::kDiagLeftBankedDown25ToFlatSeq2, Narrow::kDiagLeftBankedDown25ToFlatSeq3 }}, // DiagLeftBankedDown25ToFlat
        {{ Narrow::kDiagRightBankedDown25ToFlatSeq0, Narrow::kDiagRightBankedDown25ToFlatSeq1, Narrow::kDiagRightBankedDown25ToFlatSeq2, Narrow::kDiagRightBankedDown25ToFlatSeq3 }}, // DiagRightBankedDown25ToFlat
        {{ Narrow::kDiagUp25LeftBankedSeq0, Narrow::kDiagUp25LeftBankedSeq1, Narrow::kDiagUp25LeftBankedSeq2, Narrow::kDiagUp25LeftBankedSeq3 }}, // DiagUp25LeftBanked
        {{ Narrow::kDiagUp25RightBankedSeq0, Narrow::kDiagUp25RightBankedSeq1, Narrow::kDiagUp25RightBankedSeq2, Narrow::kDiagUp25RightBankedSeq3 }}, // DiagUp25RightBanked
        {{ Narrow::kDiagDown25LeftBankedSeq0, Narrow::kDiagDown25LeftBankedSeq1, Narrow::kDiagDown25LeftBankedSeq2, Narrow::kDiagDown25LeftBankedSeq3 }}, // DiagDown25LeftBanked
        {{ Narrow::kDiagDown25RightBankedSeq0, Narrow::kDiagDown25RightBankedSeq1, Narrow::kDiagDown25RightBankedSeq2, Narrow::kDiagDown25RightBankedSeq3 }}, // DiagDown25RightBanked
        {{ Narrow::kLeftEighthBankToDiagUp25Seq0, Narrow::kLeftEighthBankToDiagUp25Seq1, Narrow::kLeftEighthBankToDiagUp25Seq2, Narrow::kLeftEighthBankToDiagUp25Seq3, Narrow::kLeftEighthBankToDiagUp25Seq4 }}, // LeftEighthBankToDiagUp25
        {{ Narrow::kRightEighthBankToDiagUp25Seq0, Narrow::kRightEighthBankToDiagUp25Seq1, Narrow::kRightEighthBankToDiagUp25Seq2, Narrow::kRightEighthBankToDiagUp25Seq3, Narrow::kRightEighthBankToDiagUp25Seq4 }}, // RightEighthBankToDiagUp25
        {{ Narrow::kLeftEighthBankToDiagDown25Seq0, Narrow::kLeftEighthBankToDiagDown25Seq1, Narrow::kLeftEighthBankToDiagDown25Seq2, Narrow::kLeftEighthBankToDiagDown25Seq3, Narrow::kLeftEighthBankToDiagDown25Seq4 }}, // LeftEighthBankToDiagDown25
        {{ Narrow::kRightEighthBankToDiagDown25Seq0, Narrow::kRightEighthBankToDiagDown25Seq1, Narrow::kRightEighthBankToDiagDown25Seq2, Narrow::kRightEighthBankToDiagDown25Seq3, Narrow::kRightEighthBankToDiagDown25Seq4 }}, // RightEighthBankToDiagDown25
        {{ Narrow::kLeftEighthBankToOrthogonalUp25Seq0, Narrow::kLeftEighthBankToOrthogonalUp25Seq1, Narrow::kLeftEighthBankToOrthogonalUp25Seq2, Narrow::kLeftEighthBankToOrthogonalUp25Seq3, Narrow::kLeftEighthBankToOrthogonalUp25Seq4 }}, // LeftEighthBankToOrthogonalUp25
        {{ Narrow::kRightEighthBankToOrthogonalUp25Seq0, Narrow::kRightEighthBankToOrthogonalUp25Seq1, Narrow::kRightEighthBankToOrthogonalUp25Seq2, Narrow::kRightEighthBankToOrthogonalUp25Seq3, Narrow::kRightEighthBankToOrthogonalUp25Seq4 }}, // RightEighthBankToOrthogonalUp25
        {{ Narrow::kLeftEighthBankToOrthogonalDown25Seq0, Narrow::kLeftEighthBankToOrthogonalDown25Seq1, Narrow::kLeftEighthBankToOrthogonalDown25Seq2, Narrow::kLeftEighthBankToOrthogonalDown25Seq3, Narrow::kLeftEighthBankToOrthogonalDown25Seq4 }}, // LeftEighthBankToOrthogonalDown25
        {{ Narrow::kRightEighthBankToOrthogonalDown25Seq0, Narrow::kRightEighthBankToOrthogonalDown25Seq1, Narrow::kRightEighthBankToOrthogonalDown25Seq2, Narrow::kRightEighthBankToOrthogonalDown25Seq3, Narrow::kRightEighthBankToOrthogonalDown25Seq4 }}, // RightEighthBankToOrthogonalDown25
        {{ Narrow::kDiagBrakesSeq0, Narrow::kDiagBrakesSeq1, Narrow::kDiagBrakesSeq2, Narrow::kDiagBrakesSeq3 }}, // DiagBrakes
        {{ Narrow::kDiagBlockBrakesSeq0, Narrow::kDiagBlockBrakesSeq1, Narrow::kDiagBlockBrakesSeq2, Narrow::kDiagBlockBrakesSeq3 }}, // DiagBlockBrakes
        {{ Narrow::kDown25BrakesSeq0 }}, // Down25Brakes
        {{ Narrow::kDiagBoosterSeq0, Narrow::kDiagBoosterSeq1, Narrow::kDiagBoosterSeq2, Narrow::kDiagBoosterSeq3 }}, // DiagBooster
        {{ Narrow::kDiagFlatToUp60LongBaseSeq0, Narrow::kDiagFlatToUp60LongBaseSeq1, Narrow::kDiagFlatToUp60LongBaseSeq2, Narrow::kDiagFlatToUp60LongBaseSeq3, Narrow::kDiagFlatToUp60LongBaseSeq4, Narrow::kDiagFlatToUp60LongBaseSeq5, Narrow::kDiagFlatToUp60LongBaseSeq6, Narrow::kDiagFlatToUp60LongBaseSeq7, Narrow::kDiagFlatToUp60LongBaseSeq8, Narrow::kDiagFlatToUp60LongBaseSeq9 }}, // DiagFlatToUp60LongBase
        {{ Narrow::kDiagUp60ToFlatLongBaseSeq0, Narrow::kDiagUp60ToFlatLongBaseSeq1, Narrow::kDiagUp60ToFlatLongBaseSeq2, Narrow::kDiagUp60ToFlatLongBaseSeq3, Narrow::kDiagUp60ToFlatLongBaseSeq4, Narrow::kDiagUp60ToFlatLongBaseSeq5, Narrow::kDiagUp60ToFlatLongBaseSeq6, Narrow::kDiagUp60ToFlatLongBaseSeq7, Narrow::kDiagUp60ToFlatLongBaseSeq8, Narrow::kDiagUp60ToFlatLongBaseSeq9 }}, // DiagUp60ToFlatLongBase
        {{ Narrow::kDiagFlatToDown60LongBaseSeq0, Narrow::kDiagFlatToDown60LongBaseSeq1, Narrow::kDiagFlatToDown60LongBaseSeq2, Narrow::kDiagFlatToDown60LongBaseSeq3, Narrow::kDiagFlatToDown60LongBaseSeq4, Narrow::kDiagFlatToDown60LongBaseSeq5, Narrow::kDiagFlatToDown60LongBaseSeq6, Narrow::kDiagFlatToDown60LongBaseSeq7, Narrow::kDiagFlatToDown60LongBaseSeq8, Narrow::kDiagFlatToDown60LongBaseSeq9 }}, // DiagFlatToDown60LongBase
        {{ Narrow::kDiagDown60ToFlatLongBaseSeq0, Narrow::kDiagDown60ToFlatLongBaseSeq1, Narrow::kDiagDown60ToFlatLongBaseSeq2, Narrow::kDiagDown60ToFlatLongBaseSeq3, Narrow::kDiagDown60ToFlatLongBaseSeq4, Narrow::kDiagDown60ToFlatLongBaseSeq5, Narrow::kDiagDown60ToFlatLongBaseSeq6, Narrow::kDiagDown60ToFlatLongBaseSeq7, Narrow::kDiagDown60ToFlatLongBaseSeq8, Narrow::kDiagDown60ToFlatLongBaseSeq9 }}, // DiagDown60ToFlatLongBase
        {{ Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq0, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq1, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq2, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq3, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq4, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq5 }}, // LeftEighthDiveLoopUpToOrthogonal
        {{ Narrow::kRightEighthDiveLoopUpToOrthogonalSeq0, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq1, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq2, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq3, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq4, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq5 }}, // RightEighthDiveLoopUpToOrthogonal
        {{ Narrow::kLeftEighthDiveLoopDownToDiagSeq0, Narrow::kLeftEighthDiveLoopDownToDiagSeq1, Narrow::kLeftEighthDiveLoopDownToDiagSeq2, Narrow::kLeftEighthDiveLoopDownToDiagSeq3, Narrow::kLeftEighthDiveLoopDownToDiagSeq4, Narrow::kLeftEighthDiveLoopDownToDiagSeq5 }}, // LeftEighthDiveLoopDownToDiag
        {{ Narrow::kRightEighthDiveLoopDownToDiagSeq0, Narrow::kRightEighthDiveLoopDownToDiagSeq1, Narrow::kRightEighthDiveLoopDownToDiagSeq2, Narrow::kRightEighthDiveLoopDownToDiagSeq3, Narrow::kRightEighthDiveLoopDownToDiagSeq4, Narrow::kRightEighthDiveLoopDownToDiagSeq5 }}, // RightEighthDiveLoopDownToDiag
        {{ Narrow::kDiagDown25BrakesSeq0, Narrow::kDiagDown25BrakesSeq1, Narrow::kDiagDown25BrakesSeq2, Narrow::kDiagDown25BrakesSeq3 }}, // DiagDown25Brakes
    };

    static_assert(std::size(kBlockedSegmentsNarrow) == EnumValue(TrackElemType::Count));

    static constexpr const std::array<uint16_t, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> kBlockedSegmentsInverted[] = {
        {{ Inverted::kFlatSeq0 }}, // Flat
        {{ Inverted::kEndStationSeq0 }}, // EndStation
        {{ Inverted::kBeginStationSeq0 }}, // BeginStation
        {{ Inverted::kMiddleStationSeq0 }}, // MiddleStation
        {{ Inverted::kUp25Seq0 }}, // Up25
        {{ Inverted::kUp60Seq0 }}, // Up60
        {{ Inverted::kFlatToUp25Seq0 }}, // FlatToUp25
        {{ Inverted::kUp25ToUp60Seq0 }}, // Up25ToUp60
        {{ Inverted::kUp60ToUp25Seq0 }}, // Up60ToUp25
        {{ Inverted::kUp25ToFlatSeq0 }}, // Up25ToFlat
        {{ Inverted::kDown25Seq0 }}, // Down25
        {{ Inverted::kDown60Seq0 }}, // Down60
        {{ Inverted::kFlatToDown25Seq0 }}, // FlatToDown25
        {{ Inverted::kDown25ToDown60Seq0 }}, // Down25ToDown60
        {{ Inverted::kDown60ToDown25Seq0 }}, // Down60ToDown25
        {{ Inverted::kDown25ToFlatSeq0 }}, // Down25ToFlat
        {{ Inverted::kLeftQuarterTurn5TilesSeq0, Inverted::kLeftQuarterTurn5TilesSeq1, Inverted::kLeftQuarterTurn5TilesSeq2, Inverted::kLeftQuarterTurn5TilesSeq3, Inverted::kLeftQuarterTurn5TilesSeq4, Inverted::kLeftQuarterTurn5TilesSeq5, Inverted::kLeftQuarterTurn5TilesSeq6 }}, // LeftQuarterTurn5Tiles
        {{ Inverted::kRightQuarterTurn5TilesSeq0, Inverted::kRightQuarterTurn5TilesSeq1, Inverted::kRightQuarterTurn5TilesSeq2, Inverted::kRightQuarterTurn5TilesSeq3, Inverted::kRightQuarterTurn5TilesSeq4, Inverted::kRightQuarterTurn5TilesSeq5, Inverted::kRightQuarterTurn5TilesSeq6 }}, // RightQuarterTurn5Tiles
        {{ Inverted::kFlatToLeftBankSeq0 }}, // FlatToLeftBank
        {{ Inverted::kFlatToRightBankSeq0 }}, // FlatToRightBank
        {{ Inverted::kLeftBankToFlatSeq0 }}, // LeftBankToFlat
        {{ Inverted::kRightBankToFlatSeq0 }}, // RightBankToFlat
        {{ Inverted::kBankedLeftQuarterTurn5TilesSeq0, Inverted::kBankedLeftQuarterTurn5TilesSeq1, Inverted::kBankedLeftQuarterTurn5TilesSeq2, Inverted::kBankedLeftQuarterTurn5TilesSeq3, Inverted::kBankedLeftQuarterTurn5TilesSeq4, Inverted::kBankedLeftQuarterTurn5TilesSeq5, Inverted::kBankedLeftQuarterTurn5TilesSeq6 }}, // BankedLeftQuarterTurn5Tiles
        {{ Inverted::kBankedRightQuarterTurn5TilesSeq0, Inverted::kBankedRightQuarterTurn5TilesSeq1, Inverted::kBankedRightQuarterTurn5TilesSeq2, Inverted::kBankedRightQuarterTurn5TilesSeq3, Inverted::kBankedRightQuarterTurn5TilesSeq4, Inverted::kBankedRightQuarterTurn5TilesSeq5, Inverted::kBankedRightQuarterTurn5TilesSeq6 }}, // BankedRightQuarterTurn5Tiles
        {{ Inverted::kLeftBankToUp25Seq0 }}, // LeftBankToUp25
        {{ Inverted::kRightBankToUp25Seq0 }}, // RightBankToUp25
        {{ Inverted::kUp25ToLeftBankSeq0 }}, // Up25ToLeftBank
        {{ Inverted::kUp25ToRightBankSeq0 }}, // Up25ToRightBank
        {{ Inverted::kLeftBankToDown25Seq0 }}, // LeftBankToDown25
        {{ Inverted::kRightBankToDown25Seq0 }}, // RightBankToDown25
        {{ Inverted::kDown25ToLeftBankSeq0 }}, // Down25ToLeftBank
        {{ Inverted::kDown25ToRightBankSeq0 }}, // Down25ToRightBank
        {{ Inverted::kLeftBankSeq0 }}, // LeftBank
        {{ Inverted::kRightBankSeq0 }}, // RightBank
        {{ Inverted::kLeftQuarterTurn5TilesUp25Seq0, Inverted::kLeftQuarterTurn5TilesUp25Seq1, Inverted::kLeftQuarterTurn5TilesUp25Seq2, Inverted::kLeftQuarterTurn5TilesUp25Seq3, Inverted::kLeftQuarterTurn5TilesUp25Seq4, Inverted::kLeftQuarterTurn5TilesUp25Seq5, Inverted::kLeftQuarterTurn5TilesUp25Seq6 }}, // LeftQuarterTurn5TilesUp25
        {{ Inverted::kRightQuarterTurn5TilesUp25Seq0, Inverted::kRightQuarterTurn5TilesUp25Seq1, Inverted::kRightQuarterTurn5TilesUp25Seq2, Inverted::kRightQuarterTurn5TilesUp25Seq3, Inverted::kRightQuarterTurn5TilesUp25Seq4, Inverted::kRightQuarterTurn5TilesUp25Seq5, Inverted::kRightQuarterTurn5TilesUp25Seq6 }}, // RightQuarterTurn5TilesUp25
        {{ Inverted::kLeftQuarterTurn5TilesDown25Seq0, Inverted::kLeftQuarterTurn5TilesDown25Seq1, Inverted::kLeftQuarterTurn5TilesDown25Seq2, Inverted::kLeftQuarterTurn5TilesDown25Seq3, Inverted::kLeftQuarterTurn5TilesDown25Seq4, Inverted::kLeftQuarterTurn5TilesDown25Seq5, Inverted::kLeftQuarterTurn5TilesDown25Seq6 }}, // LeftQuarterTurn5TilesDown25
        {{ Inverted::kRightQuarterTurn5TilesDown25Seq0, Inverted::kRightQuarterTurn5TilesDown25Seq1, Inverted::kRightQuarterTurn5TilesDown25Seq2, Inverted::kRightQuarterTurn5TilesDown25Seq3, Inverted::kRightQuarterTurn5TilesDown25Seq4, Inverted::kRightQuarterTurn5TilesDown25Seq5, Inverted::kRightQuarterTurn5TilesDown25Seq6 }}, // RightQuarterTurn5TilesDown25
        {{ Inverted::kSBendLeftSeq0, Inverted::kSBendLeftSeq1, Inverted::kSBendLeftSeq2, Inverted::kSBendLeftSeq3 }}, // SBendLeft
        {{ Inverted::kSBendRightSeq0, Inverted::kSBendRightSeq1, Inverted::kSBendRightSeq2, Inverted::kSBendRightSeq3 }}, // SBendRight
        {{ Inverted::kLeftVerticalLoopSeq0, Inverted::kLeftVerticalLoopSeq1, Inverted::kLeftVerticalLoopSeq2, Inverted::kLeftVerticalLoopSeq3, Inverted::kLeftVerticalLoopSeq4, Inverted::kLeftVerticalLoopSeq5, Inverted::kLeftVerticalLoopSeq6, Inverted::kLeftVerticalLoopSeq7, Inverted::kLeftVerticalLoopSeq8, Inverted::kLeftVerticalLoopSeq9 }}, // LeftVerticalLoop
        {{ Inverted::kRightVerticalLoopSeq0, Inverted::kRightVerticalLoopSeq1, Inverted::kRightVerticalLoopSeq2, Inverted::kRightVerticalLoopSeq3, Inverted::kRightVerticalLoopSeq4, Inverted::kRightVerticalLoopSeq5, Inverted::kRightVerticalLoopSeq6, Inverted::kRightVerticalLoopSeq7, Inverted::kRightVerticalLoopSeq8, Inverted::kRightVerticalLoopSeq9 }}, // RightVerticalLoop
        {{ Inverted::kLeftQuarterTurn3TilesSeq0, Inverted::kLeftQuarterTurn3TilesSeq1, Inverted::kLeftQuarterTurn3TilesSeq2, Inverted::kLeftQuarterTurn3TilesSeq3 }}, // LeftQuarterTurn3Tiles
        {{ Inverted::kRightQuarterTurn3TilesSeq0, Inverted::kRightQuarterTurn3TilesSeq1, Inverted::kRightQuarterTurn3TilesSeq2, Inverted::kRightQuarterTurn3TilesSeq3 }}, // RightQuarterTurn3Tiles
        {{ Inverted::kLeftBankedQuarterTurn3TilesSeq0, Inverted::kLeftBankedQuarterTurn3TilesSeq1, Inverted::kLeftBankedQuarterTurn3TilesSeq2, Inverted::kLeftBankedQuarterTurn3TilesSeq3 }}, // LeftBankedQuarterTurn3Tiles
        {{ Inverted::kRightBankedQuarterTurn3TilesSeq0, Inverted::kRightBankedQuarterTurn3TilesSeq1, Inverted::kRightBankedQuarterTurn3TilesSeq2, Inverted::kRightBankedQuarterTurn3TilesSeq3 }}, // RightBankedQuarterTurn3Tiles
        {{ Inverted::kLeftQuarterTurn3TilesUp25Seq0, Inverted::kLeftQuarterTurn3TilesUp25Seq1, Inverted::kLeftQuarterTurn3TilesUp25Seq2, Inverted::kLeftQuarterTurn3TilesUp25Seq3 }}, // LeftQuarterTurn3TilesUp25
        {{ Inverted::kRightQuarterTurn3TilesUp25Seq0, Inverted::kRightQuarterTurn3TilesUp25Seq1, Inverted::kRightQuarterTurn3TilesUp25Seq2, Inverted::kRightQuarterTurn3TilesUp25Seq3 }}, // RightQuarterTurn3TilesUp25
        {{ Inverted::kLeftQuarterTurn3TilesDown25Seq0, Inverted::kLeftQuarterTurn3TilesDown25Seq1, Inverted::kLeftQuarterTurn3TilesDown25Seq2, Inverted::kLeftQuarterTurn3TilesDown25Seq3 }}, // LeftQuarterTurn3TilesDown25
        {{ Inverted::kRightQuarterTurn3TilesDown25Seq0, Inverted::kRightQuarterTurn3TilesDown25Seq1, Inverted::kRightQuarterTurn3TilesDown25Seq2, Inverted::kRightQuarterTurn3TilesDown25Seq3 }}, // RightQuarterTurn3TilesDown25
        {{ Inverted::kLeftQuarterTurn1TileSeq0 }}, // LeftQuarterTurn1Tile
        {{ Inverted::kRightQuarterTurn1TileSeq0 }}, // RightQuarterTurn1Tile
        {{ Inverted::kLeftTwistDownToUpSeq0, Inverted::kLeftTwistDownToUpSeq1, Inverted::kLeftTwistDownToUpSeq2 }}, // LeftTwistDownToUp
        {{ Inverted::kRightTwistDownToUpSeq0, Inverted::kRightTwistDownToUpSeq1, Inverted::kRightTwistDownToUpSeq2 }}, // RightTwistDownToUp
        {{ Inverted::kLeftTwistUpToDownSeq0, Inverted::kLeftTwistUpToDownSeq1, Inverted::kLeftTwistUpToDownSeq2 }}, // LeftTwistUpToDown
        {{ Inverted::kRightTwistUpToDownSeq0, Inverted::kRightTwistUpToDownSeq1, Inverted::kRightTwistUpToDownSeq2 }}, // RightTwistUpToDown
        {{ Inverted::kHalfLoopUpSeq0, Inverted::kHalfLoopUpSeq1, Inverted::kHalfLoopUpSeq2, Inverted::kHalfLoopUpSeq3 }}, // HalfLoopUp
        {{ Inverted::kHalfLoopDownSeq0, Inverted::kHalfLoopDownSeq1, Inverted::kHalfLoopDownSeq2, Inverted::kHalfLoopDownSeq3 }}, // HalfLoopDown
        {{ Inverted::kLeftCorkscrewUpSeq0, Inverted::kLeftCorkscrewUpSeq1, Inverted::kLeftCorkscrewUpSeq2 }}, // LeftCorkscrewUp
        {{ Inverted::kRightCorkscrewUpSeq0, Inverted::kRightCorkscrewUpSeq1, Inverted::kRightCorkscrewUpSeq2 }}, // RightCorkscrewUp
        {{ Inverted::kLeftCorkscrewDownSeq0, Inverted::kLeftCorkscrewDownSeq1, Inverted::kLeftCorkscrewDownSeq2 }}, // LeftCorkscrewDown
        {{ Inverted::kRightCorkscrewDownSeq0, Inverted::kRightCorkscrewDownSeq1, Inverted::kRightCorkscrewDownSeq2 }}, // RightCorkscrewDown
        {{ Inverted::kFlatToUp60Seq0 }}, // FlatToUp60
        {{ Inverted::kUp60ToFlatSeq0 }}, // Up60ToFlat
        {{ Inverted::kFlatToDown60Seq0 }}, // FlatToDown60
        {{ Inverted::kDown60ToFlatSeq0 }}, // Down60ToFlat
        {{ Inverted::kTowerBaseSeq0, Inverted::kTowerBaseSeq1, Inverted::kTowerBaseSeq2, Inverted::kTowerBaseSeq3, Inverted::kTowerBaseSeq4, Inverted::kTowerBaseSeq5, Inverted::kTowerBaseSeq6, Inverted::kTowerBaseSeq7, Inverted::kTowerBaseSeq8 }}, // TowerBase
        {{ Inverted::kTowerSectionSeq0, Inverted::kTowerSectionSeq1 }}, // TowerSection
        {{ Inverted::kFlatCoveredSeq0 }}, // FlatCovered
        {{ Inverted::kUp25CoveredSeq0 }}, // Up25Covered
        {{ Inverted::kUp60CoveredSeq0 }}, // Up60Covered
        {{ Inverted::kFlatToUp25CoveredSeq0 }}, // FlatToUp25Covered
        {{ Inverted::kUp25ToUp60CoveredSeq0 }}, // Up25ToUp60Covered
        {{ Inverted::kUp60ToUp25CoveredSeq0 }}, // Up60ToUp25Covered
        {{ Inverted::kUp25ToFlatCoveredSeq0 }}, // Up25ToFlatCovered
        {{ Inverted::kDown25CoveredSeq0 }}, // Down25Covered
        {{ Inverted::kDown60CoveredSeq0 }}, // Down60Covered
        {{ Inverted::kFlatToDown25CoveredSeq0 }}, // FlatToDown25Covered
        {{ Inverted::kDown25ToDown60CoveredSeq0 }}, // Down25ToDown60Covered
        {{ Inverted::kDown60ToDown25CoveredSeq0 }}, // Down60ToDown25Covered
        {{ Inverted::kDown25ToFlatCoveredSeq0 }}, // Down25ToFlatCovered
        {{ Inverted::kLeftQuarterTurn5TilesCoveredSeq0, Inverted::kLeftQuarterTurn5TilesCoveredSeq1, Inverted::kLeftQuarterTurn5TilesCoveredSeq2, Inverted::kLeftQuarterTurn5TilesCoveredSeq3, Inverted::kLeftQuarterTurn5TilesCoveredSeq4, Inverted::kLeftQuarterTurn5TilesCoveredSeq5, Inverted::kLeftQuarterTurn5TilesCoveredSeq6 }}, // LeftQuarterTurn5TilesCovered
        {{ Inverted::kRightQuarterTurn5TilesCoveredSeq0, Inverted::kRightQuarterTurn5TilesCoveredSeq1, Inverted::kRightQuarterTurn5TilesCoveredSeq2, Inverted::kRightQuarterTurn5TilesCoveredSeq3, Inverted::kRightQuarterTurn5TilesCoveredSeq4, Inverted::kRightQuarterTurn5TilesCoveredSeq5, Inverted::kRightQuarterTurn5TilesCoveredSeq6 }}, // RightQuarterTurn5TilesCovered
        {{ Inverted::kSBendLeftCoveredSeq0, Inverted::kSBendLeftCoveredSeq1, Inverted::kSBendLeftCoveredSeq2, Inverted::kSBendLeftCoveredSeq3 }}, // SBendLeftCovered
        {{ Inverted::kSBendRightCoveredSeq0, Inverted::kSBendRightCoveredSeq1, Inverted::kSBendRightCoveredSeq2, Inverted::kSBendRightCoveredSeq3 }}, // SBendRightCovered
        {{ Inverted::kLeftQuarterTurn3TilesCoveredSeq0, Inverted::kLeftQuarterTurn3TilesCoveredSeq1, Inverted::kLeftQuarterTurn3TilesCoveredSeq2, Inverted::kLeftQuarterTurn3TilesCoveredSeq3 }}, // LeftQuarterTurn3TilesCovered
        {{ Inverted::kRightQuarterTurn3TilesCoveredSeq0, Inverted::kRightQuarterTurn3TilesCoveredSeq1, Inverted::kRightQuarterTurn3TilesCoveredSeq2, Inverted::kRightQuarterTurn3TilesCoveredSeq3 }}, // RightQuarterTurn3TilesCovered
        {{ Inverted::kLeftHalfBankedHelixUpSmallSeq0, Inverted::kLeftHalfBankedHelixUpSmallSeq1, Inverted::kLeftHalfBankedHelixUpSmallSeq2, Inverted::kLeftHalfBankedHelixUpSmallSeq3, Inverted::kLeftHalfBankedHelixUpSmallSeq4, Inverted::kLeftHalfBankedHelixUpSmallSeq5, Inverted::kLeftHalfBankedHelixUpSmallSeq6, Inverted::kLeftHalfBankedHelixUpSmallSeq7 }}, // LeftHalfBankedHelixUpSmall
        {{ Inverted::kRightHalfBankedHelixUpSmallSeq0, Inverted::kRightHalfBankedHelixUpSmallSeq1, Inverted::kRightHalfBankedHelixUpSmallSeq2, Inverted::kRightHalfBankedHelixUpSmallSeq3, Inverted::kRightHalfBankedHelixUpSmallSeq4, Inverted::kRightHalfBankedHelixUpSmallSeq5, Inverted::kRightHalfBankedHelixUpSmallSeq6, Inverted::kRightHalfBankedHelixUpSmallSeq7 }}, // RightHalfBankedHelixUpSmall
        {{ Inverted::kLeftHalfBankedHelixDownSmallSeq0, Inverted::kLeftHalfBankedHelixDownSmallSeq1, Inverted::kLeftHalfBankedHelixDownSmallSeq2, Inverted::kLeftHalfBankedHelixDownSmallSeq3, Inverted::kLeftHalfBankedHelixDownSmallSeq4, Inverted::kLeftHalfBankedHelixDownSmallSeq5, Inverted::kLeftHalfBankedHelixDownSmallSeq6, Inverted::kLeftHalfBankedHelixDownSmallSeq7 }}, // LeftHalfBankedHelixDownSmall
        {{ Inverted::kRightHalfBankedHelixDownSmallSeq0, Inverted::kRightHalfBankedHelixDownSmallSeq1, Inverted::kRightHalfBankedHelixDownSmallSeq2, Inverted::kRightHalfBankedHelixDownSmallSeq3, Inverted::kRightHalfBankedHelixDownSmallSeq4, Inverted::kRightHalfBankedHelixDownSmallSeq5, Inverted::kRightHalfBankedHelixDownSmallSeq6, Inverted::kRightHalfBankedHelixDownSmallSeq7 }}, // RightHalfBankedHelixDownSmall
        {{ Inverted::kLeftHalfBankedHelixUpLargeSeq0, Inverted::kLeftHalfBankedHelixUpLargeSeq1, Inverted::kLeftHalfBankedHelixUpLargeSeq2, Inverted::kLeftHalfBankedHelixUpLargeSeq3, Inverted::kLeftHalfBankedHelixUpLargeSeq4, Inverted::kLeftHalfBankedHelixUpLargeSeq5, Inverted::kLeftHalfBankedHelixUpLargeSeq6, Inverted::kLeftHalfBankedHelixUpLargeSeq7, Inverted::kLeftHalfBankedHelixUpLargeSeq8, Inverted::kLeftHalfBankedHelixUpLargeSeq9, Inverted::kLeftHalfBankedHelixUpLargeSeq10, Inverted::kLeftHalfBankedHelixUpLargeSeq11, Inverted::kLeftHalfBankedHelixUpLargeSeq12, Inverted::kLeftHalfBankedHelixUpLargeSeq13 }}, // LeftHalfBankedHelixUpLarge
        {{ Inverted::kRightHalfBankedHelixUpLargeSeq0, Inverted::kRightHalfBankedHelixUpLargeSeq1, Inverted::kRightHalfBankedHelixUpLargeSeq2, Inverted::kRightHalfBankedHelixUpLargeSeq3, Inverted::kRightHalfBankedHelixUpLargeSeq4, Inverted::kRightHalfBankedHelixUpLargeSeq5, Inverted::kRightHalfBankedHelixUpLargeSeq6, Inverted::kRightHalfBankedHelixUpLargeSeq7, Inverted::kRightHalfBankedHelixUpLargeSeq8, Inverted::kRightHalfBankedHelixUpLargeSeq9, Inverted::kRightHalfBankedHelixUpLargeSeq10, Inverted::kRightHalfBankedHelixUpLargeSeq11, Inverted::kRightHalfBankedHelixUpLargeSeq12, Inverted::kRightHalfBankedHelixUpLargeSeq13 }}, // RightHalfBankedHelixUpLarge
        {{ Inverted::kLeftHalfBankedHelixDownLargeSeq0, Inverted::kLeftHalfBankedHelixDownLargeSeq1, Inverted::kLeftHalfBankedHelixDownLargeSeq2, Inverted::kLeftHalfBankedHelixDownLargeSeq3, Inverted::kLeftHalfBankedHelixDownLargeSeq4, Inverted::kLeftHalfBankedHelixDownLargeSeq5, Inverted::kLeftHalfBankedHelixDownLargeSeq6, Inverted::kLeftHalfBankedHelixDownLargeSeq7, Inverted::kLeftHalfBankedHelixDownLargeSeq8, Inverted::kLeftHalfBankedHelixDownLargeSeq9, Inverted::kLeftHalfBankedHelixDownLargeSeq10, Inverted::kLeftHalfBankedHelixDownLargeSeq11, Inverted::kLeftHalfBankedHelixDownLargeSeq12, Inverted::kLeftHalfBankedHelixDownLargeSeq13 }}, // LeftHalfBankedHelixDownLarge
        {{ Inverted::kRightHalfBankedHelixDownLargeSeq0, Inverted::kRightHalfBankedHelixDownLargeSeq1, Inverted::kRightHalfBankedHelixDownLargeSeq2, Inverted::kRightHalfBankedHelixDownLargeSeq3, Inverted::kRightHalfBankedHelixDownLargeSeq4, Inverted::kRightHalfBankedHelixDownLargeSeq5, Inverted::kRightHalfBankedHelixDownLargeSeq6, Inverted::kRightHalfBankedHelixDownLargeSeq7, Inverted::kRightHalfBankedHelixDownLargeSeq8, Inverted::kRightHalfBankedHelixDownLargeSeq9, Inverted::kRightHalfBankedHelixDownLargeSeq10, Inverted::kRightHalfBankedHelixDownLargeSeq11, Inverted::kRightHalfBankedHelixDownLargeSeq12, Inverted::kRightHalfBankedHelixDownLargeSeq13 }}, // RightHalfBankedHelixDownLarge
        {{ Inverted::kLeftQuarterTurn1TileUp60Seq0 }}, // LeftQuarterTurn1TileUp60
        {{ Inverted::kRightQuarterTurn1TileUp60Seq0 }}, // RightQuarterTurn1TileUp60
        {{ Inverted::kLeftQuarterTurn1TileDown60Seq0 }}, // LeftQuarterTurn1TileDown60
        {{ Inverted::kRightQuarterTurn1TileDown60Seq0 }}, // RightQuarterTurn1TileDown60
        {{ Inverted::kBrakesSeq0 }}, // Brakes
        {{ Inverted::kBoosterSeq0 }}, // Booster
        {{ Inverted::kMazeSeq0 }}, // Maze
        {{ Inverted::kLeftQuarterBankedHelixLargeUpSeq0, Inverted::kLeftQuarterBankedHelixLargeUpSeq1, Inverted::kLeftQuarterBankedHelixLargeUpSeq2, Inverted::kLeftQuarterBankedHelixLargeUpSeq3, Inverted::kLeftQuarterBankedHelixLargeUpSeq4, Inverted::kLeftQuarterBankedHelixLargeUpSeq5, Inverted::kLeftQuarterBankedHelixLargeUpSeq6 }}, // LeftQuarterBankedHelixLargeUp
        {{ Inverted::kRightQuarterBankedHelixLargeUpSeq0, Inverted::kRightQuarterBankedHelixLargeUpSeq1, Inverted::kRightQuarterBankedHelixLargeUpSeq2, Inverted::kRightQuarterBankedHelixLargeUpSeq3, Inverted::kRightQuarterBankedHelixLargeUpSeq4, Inverted::kRightQuarterBankedHelixLargeUpSeq5, Inverted::kRightQuarterBankedHelixLargeUpSeq6 }}, // RightQuarterBankedHelixLargeUp
        {{ Inverted::kLeftQuarterBankedHelixLargeDownSeq0, Inverted::kLeftQuarterBankedHelixLargeDownSeq1, Inverted::kLeftQuarterBankedHelixLargeDownSeq2, Inverted::kLeftQuarterBankedHelixLargeDownSeq3, Inverted::kLeftQuarterBankedHelixLargeDownSeq4, Inverted::kLeftQuarterBankedHelixLargeDownSeq5, Inverted::kLeftQuarterBankedHelixLargeDownSeq6 }}, // LeftQuarterBankedHelixLargeDown
        {{ Inverted::kRightQuarterBankedHelixLargeDownSeq0, Inverted::kRightQuarterBankedHelixLargeDownSeq1, Inverted::kRightQuarterBankedHelixLargeDownSeq2, Inverted::kRightQuarterBankedHelixLargeDownSeq3, Inverted::kRightQuarterBankedHelixLargeDownSeq4, Inverted::kRightQuarterBankedHelixLargeDownSeq5, Inverted::kRightQuarterBankedHelixLargeDownSeq6 }}, // RightQuarterBankedHelixLargeDown
        {{ Inverted::kLeftQuarterHelixLargeUpSeq0, Inverted::kLeftQuarterHelixLargeUpSeq1, Inverted::kLeftQuarterHelixLargeUpSeq2, Inverted::kLeftQuarterHelixLargeUpSeq3, Inverted::kLeftQuarterHelixLargeUpSeq4, Inverted::kLeftQuarterHelixLargeUpSeq5, Inverted::kLeftQuarterHelixLargeUpSeq6 }}, // LeftQuarterHelixLargeUp
        {{ Inverted::kRightQuarterHelixLargeUpSeq0, Inverted::kRightQuarterHelixLargeUpSeq1, Inverted::kRightQuarterHelixLargeUpSeq2, Inverted::kRightQuarterHelixLargeUpSeq3, Inverted::kRightQuarterHelixLargeUpSeq4, Inverted::kRightQuarterHelixLargeUpSeq5, Inverted::kRightQuarterHelixLargeUpSeq6 }}, // RightQuarterHelixLargeUp
        {{ Inverted::kLeftQuarterHelixLargeDownSeq0, Inverted::kLeftQuarterHelixLargeDownSeq1, Inverted::kLeftQuarterHelixLargeDownSeq2, Inverted::kLeftQuarterHelixLargeDownSeq3, Inverted::kLeftQuarterHelixLargeDownSeq4, Inverted::kLeftQuarterHelixLargeDownSeq5, Inverted::kLeftQuarterHelixLargeDownSeq6 }}, // LeftQuarterHelixLargeDown
        {{ Inverted::kRightQuarterHelixLargeDownSeq0, Inverted::kRightQuarterHelixLargeDownSeq1, Inverted::kRightQuarterHelixLargeDownSeq2, Inverted::kRightQuarterHelixLargeDownSeq3, Inverted::kRightQuarterHelixLargeDownSeq4, Inverted::kRightQuarterHelixLargeDownSeq5, Inverted::kRightQuarterHelixLargeDownSeq6 }}, // RightQuarterHelixLargeDown
        {{ Inverted::kUp25LeftBankedSeq0 }}, // Up25LeftBanked
        {{ Inverted::kUp25RightBankedSeq0 }}, // Up25RightBanked
        {{ Inverted::kWaterfallSeq0 }}, // Waterfall
        {{ Inverted::kRapidsSeq0 }}, // Rapids
        {{ Inverted::kOnRidePhotoSeq0 }}, // OnRidePhoto
        {{ Inverted::kDown25LeftBankedSeq0 }}, // Down25LeftBanked
        {{ Inverted::kDown25RightBankedSeq0 }}, // Down25RightBanked
        {{ Inverted::kWatersplashSeq0, Inverted::kWatersplashSeq1, Inverted::kWatersplashSeq2, Inverted::kWatersplashSeq3, Inverted::kWatersplashSeq4 }}, // Watersplash
        {{ Inverted::kFlatToUp60LongBaseSeq0, Inverted::kFlatToUp60LongBaseSeq1, Inverted::kFlatToUp60LongBaseSeq2, Inverted::kFlatToUp60LongBaseSeq3 }}, // FlatToUp60LongBase
        {{ Inverted::kUp60ToFlatLongBaseSeq0, Inverted::kUp60ToFlatLongBaseSeq1, Inverted::kUp60ToFlatLongBaseSeq2, Inverted::kUp60ToFlatLongBaseSeq3 }}, // Up60ToFlatLongBase
        {{ Inverted::kWhirlpoolSeq0 }}, // Whirlpool
        {{ Inverted::kDown60ToFlatLongBaseSeq0, Inverted::kDown60ToFlatLongBaseSeq1, Inverted::kDown60ToFlatLongBaseSeq2, Inverted::kDown60ToFlatLongBaseSeq3 }}, // Down60ToFlatLongBase
        {{ Inverted::kFlatToDown60LongBaseSeq0, Inverted::kFlatToDown60LongBaseSeq1, Inverted::kFlatToDown60LongBaseSeq2, Inverted::kFlatToDown60LongBaseSeq3 }}, // FlatToDown60LongBase
        {{ Inverted::kCableLiftHillSeq0, Inverted::kCableLiftHillSeq1, Inverted::kCableLiftHillSeq2, Inverted::kCableLiftHillSeq3 }}, // CableLiftHill
        {{ Inverted::kReverseFreefallSlopeSeq0, Inverted::kReverseFreefallSlopeSeq1, Inverted::kReverseFreefallSlopeSeq2, Inverted::kReverseFreefallSlopeSeq3, Inverted::kReverseFreefallSlopeSeq4, Inverted::kReverseFreefallSlopeSeq5, Inverted::kReverseFreefallSlopeSeq6 }}, // ReverseFreefallSlope
        {{ Inverted::kReverseFreefallVerticalSeq0, Inverted::kReverseFreefallVerticalSeq1 }}, // ReverseFreefallVertical
        {{ Inverted::kUp90Seq0, Inverted::kUp90Seq1 }}, // Up90
        {{ Inverted::kDown90Seq0, Inverted::kDown90Seq1 }}, // Down90
        {{ Inverted::kUp60ToUp90Seq0, Inverted::kUp60ToUp90Seq1 }}, // Up60ToUp90
        {{ Inverted::kDown90ToDown60Seq0 }}, // Down90ToDown60
        {{ Inverted::kUp90ToUp60Seq0 }}, // Up90ToUp60
        {{ Inverted::kDown60ToDown90Seq0, Inverted::kDown60ToDown90Seq1 }}, // Down60ToDown90
        {{ Inverted::kBrakeForDropSeq0 }}, // BrakeForDrop
        {{ Inverted::kLeftEighthToDiagSeq0, Inverted::kLeftEighthToDiagSeq1, Inverted::kLeftEighthToDiagSeq2, Inverted::kLeftEighthToDiagSeq3, Inverted::kLeftEighthToDiagSeq4 }}, // LeftEighthToDiag
        {{ Inverted::kRightEighthToDiagSeq0, Inverted::kRightEighthToDiagSeq1, Inverted::kRightEighthToDiagSeq2, Inverted::kRightEighthToDiagSeq3, Inverted::kRightEighthToDiagSeq4 }}, // RightEighthToDiag
        {{ Inverted::kLeftEighthToOrthogonalSeq0, Inverted::kLeftEighthToOrthogonalSeq1, Inverted::kLeftEighthToOrthogonalSeq2, Inverted::kLeftEighthToOrthogonalSeq3, Inverted::kLeftEighthToOrthogonalSeq4 }}, // LeftEighthToOrthogonal
        {{ Inverted::kRightEighthToOrthogonalSeq0, Inverted::kRightEighthToOrthogonalSeq1, Inverted::kRightEighthToOrthogonalSeq2, Inverted::kRightEighthToOrthogonalSeq3, Inverted::kRightEighthToOrthogonalSeq4 }}, // RightEighthToOrthogonal
        {{ Inverted::kLeftEighthBankToDiagSeq0, Inverted::kLeftEighthBankToDiagSeq1, Inverted::kLeftEighthBankToDiagSeq2, Inverted::kLeftEighthBankToDiagSeq3, Inverted::kLeftEighthBankToDiagSeq4 }}, // LeftEighthBankToDiag
        {{ Inverted::kRightEighthBankToDiagSeq0, Inverted::kRightEighthBankToDiagSeq1, Inverted::kRightEighthBankToDiagSeq2, Inverted::kRightEighthBankToDiagSeq3, Inverted::kRightEighthBankToDiagSeq4 }}, // RightEighthBankToDiag
        {{ Inverted::kLeftEighthBankToOrthogonalSeq0, Inverted::kLeftEighthBankToOrthogonalSeq1, Inverted::kLeftEighthBankToOrthogonalSeq2, Inverted::kLeftEighthBankToOrthogonalSeq3, Inverted::kLeftEighthBankToOrthogonalSeq4 }}, // LeftEighthBankToOrthogonal
        {{ Inverted::kRightEighthBankToOrthogonalSeq0, Inverted::kRightEighthBankToOrthogonalSeq1, Inverted::kRightEighthBankToOrthogonalSeq2, Inverted::kRightEighthBankToOrthogonalSeq3, Inverted::kRightEighthBankToOrthogonalSeq4 }}, // RightEighthBankToOrthogonal
        {{ Inverted::kDiagFlatSeq0, Inverted::kDiagFlatSeq1, Inverted::kDiagFlatSeq2, Inverted::kDiagFlatSeq3 }}, // DiagFlat
        {{ Inverted::kDiagUp25Seq0, Inverted::kDiagUp25Seq1, Inverted::kDiagUp25Seq2, Inverted::kDiagUp25Seq3 }}, // DiagUp25
        {{ Inverted::kDiagUp60Seq0, Inverted::kDiagUp60Seq1, Inverted::kDiagUp60Seq2, Inverted::kDiagUp60Seq3 }}, // DiagUp60
        {{ Inverted::kDiagFlatToUp25Seq0, Inverted::kDiagFlatToUp25Seq1, Inverted::kDiagFlatToUp25Seq2, Inverted::kDiagFlatToUp25Seq3 }}, // DiagFlatToUp25
        {{ Inverted::kDiagUp25ToUp60Seq0, Inverted::kDiagUp25ToUp60Seq1, Inverted::kDiagUp25ToUp60Seq2, Inverted::kDiagUp25ToUp60Seq3 }}, // DiagUp25ToUp60
        {{ Inverted::kDiagUp60ToUp25Seq0, Inverted::kDiagUp60ToUp25Seq1, Inverted::kDiagUp60ToUp25Seq2, Inverted::kDiagUp60ToUp25Seq3 }}, // DiagUp60ToUp25
        {{ Inverted::kDiagUp25ToFlatSeq0, Inverted::kDiagUp25ToFlatSeq1, Inverted::kDiagUp25ToFlatSeq2, Inverted::kDiagUp25ToFlatSeq3 }}, // DiagUp25ToFlat
        {{ Inverted::kDiagDown25Seq0, Inverted::kDiagDown25Seq1, Inverted::kDiagDown25Seq2, Inverted::kDiagDown25Seq3 }}, // DiagDown25
        {{ Inverted::kDiagDown60Seq0, Inverted::kDiagDown60Seq1, Inverted::kDiagDown60Seq2, Inverted::kDiagDown60Seq3 }}, // DiagDown60
        {{ Inverted::kDiagFlatToDown25Seq0, Inverted::kDiagFlatToDown25Seq1, Inverted::kDiagFlatToDown25Seq2, Inverted::kDiagFlatToDown25Seq3 }}, // DiagFlatToDown25
        {{ Inverted::kDiagDown25ToDown60Seq0, Inverted::kDiagDown25ToDown60Seq1, Inverted::kDiagDown25ToDown60Seq2, Inverted::kDiagDown25ToDown60Seq3 }}, // DiagDown25ToDown60
        {{ Inverted::kDiagDown60ToDown25Seq0, Inverted::kDiagDown60ToDown25Seq1, Inverted::kDiagDown60ToDown25Seq2, Inverted::kDiagDown60ToDown25Seq3 }}, // DiagDown60ToDown25
        {{ Inverted::kDiagDown25ToFlatSeq0, Inverted::kDiagDown25ToFlatSeq1, Inverted::kDiagDown25ToFlatSeq2, Inverted::kDiagDown25ToFlatSeq3 }}, // DiagDown25ToFlat
        {{ Inverted::kDiagFlatToUp60Seq0, Inverted::kDiagFlatToUp60Seq1, Inverted::kDiagFlatToUp60Seq2, Inverted::kDiagFlatToUp60Seq3 }}, // DiagFlatToUp60
        {{ Inverted::kDiagUp60ToFlatSeq0, Inverted::kDiagUp60ToFlatSeq1, Inverted::kDiagUp60ToFlatSeq2, Inverted::kDiagUp60ToFlatSeq3 }}, // DiagUp60ToFlat
        {{ Inverted::kDiagFlatToDown60Seq0, Inverted::kDiagFlatToDown60Seq1, Inverted::kDiagFlatToDown60Seq2, Inverted::kDiagFlatToDown60Seq3 }}, // DiagFlatToDown60
        {{ Inverted::kDiagDown60ToFlatSeq0, Inverted::kDiagDown60ToFlatSeq1, Inverted::kDiagDown60ToFlatSeq2, Inverted::kDiagDown60ToFlatSeq3 }}, // DiagDown60ToFlat
        {{ Inverted::kDiagFlatToLeftBankSeq0, Inverted::kDiagFlatToLeftBankSeq1, Inverted::kDiagFlatToLeftBankSeq2, Inverted::kDiagFlatToLeftBankSeq3 }}, // DiagFlatToLeftBank
        {{ Inverted::kDiagFlatToRightBankSeq0, Inverted::kDiagFlatToRightBankSeq1, Inverted::kDiagFlatToRightBankSeq2, Inverted::kDiagFlatToRightBankSeq3 }}, // DiagFlatToRightBank
        {{ Inverted::kDiagLeftBankToFlatSeq0, Inverted::kDiagLeftBankToFlatSeq1, Inverted::kDiagLeftBankToFlatSeq2, Inverted::kDiagLeftBankToFlatSeq3 }}, // DiagLeftBankToFlat
        {{ Inverted::kDiagRightBankToFlatSeq0, Inverted::kDiagRightBankToFlatSeq1, Inverted::kDiagRightBankToFlatSeq2, Inverted::kDiagRightBankToFlatSeq3 }}, // DiagRightBankToFlat
        {{ Inverted::kDiagLeftBankToUp25Seq0, Inverted::kDiagLeftBankToUp25Seq1, Inverted::kDiagLeftBankToUp25Seq2, Inverted::kDiagLeftBankToUp25Seq3 }}, // DiagLeftBankToUp25
        {{ Inverted::kDiagRightBankToUp25Seq0, Inverted::kDiagRightBankToUp25Seq1, Inverted::kDiagRightBankToUp25Seq2, Inverted::kDiagRightBankToUp25Seq3 }}, // DiagRightBankToUp25
        {{ Inverted::kDiagUp25ToLeftBankSeq0, Inverted::kDiagUp25ToLeftBankSeq1, Inverted::kDiagUp25ToLeftBankSeq2, Inverted::kDiagUp25ToLeftBankSeq3 }}, // DiagUp25ToLeftBank
        {{ Inverted::kDiagUp25ToRightBankSeq0, Inverted::kDiagUp25ToRightBankSeq1, Inverted::kDiagUp25ToRightBankSeq2, Inverted::kDiagUp25ToRightBankSeq3 }}, // DiagUp25ToRightBank
        {{ Inverted::kDiagLeftBankToDown25Seq0, Inverted::kDiagLeftBankToDown25Seq1, Inverted::kDiagLeftBankToDown25Seq2, Inverted::kDiagLeftBankToDown25Seq3 }}, // DiagLeftBankToDown25
        {{ Inverted::kDiagRightBankToDown25Seq0, Inverted::kDiagRightBankToDown25Seq1, Inverted::kDiagRightBankToDown25Seq2, Inverted::kDiagRightBankToDown25Seq3 }}, // DiagRightBankToDown25
        {{ Inverted::kDiagDown25ToLeftBankSeq0, Inverted::kDiagDown25ToLeftBankSeq1, Inverted::kDiagDown25ToLeftBankSeq2, Inverted::kDiagDown25ToLeftBankSeq3 }}, // DiagDown25ToLeftBank
        {{ Inverted::kDiagDown25ToRightBankSeq0, Inverted::kDiagDown25ToRightBankSeq1, Inverted::kDiagDown25ToRightBankSeq2, Inverted::kDiagDown25ToRightBankSeq3 }}, // DiagDown25ToRightBank
        {{ Inverted::kDiagLeftBankSeq0, Inverted::kDiagLeftBankSeq1, Inverted::kDiagLeftBankSeq2, Inverted::kDiagLeftBankSeq3 }}, // DiagLeftBank
        {{ Inverted::kDiagRightBankSeq0, Inverted::kDiagRightBankSeq1, Inverted::kDiagRightBankSeq2, Inverted::kDiagRightBankSeq3 }}, // DiagRightBank
        {{ Inverted::kLogFlumeReverserSeq0 }}, // LogFlumeReverser
        {{ Inverted::kSpinningTunnelSeq0 }}, // SpinningTunnel
        {{ Inverted::kLeftBarrelRollUpToDownSeq0, Inverted::kLeftBarrelRollUpToDownSeq1, Inverted::kLeftBarrelRollUpToDownSeq2 }}, // LeftBarrelRollUpToDown
        {{ Inverted::kRightBarrelRollUpToDownSeq0, Inverted::kRightBarrelRollUpToDownSeq1, Inverted::kRightBarrelRollUpToDownSeq2 }}, // RightBarrelRollUpToDown
        {{ Inverted::kLeftBarrelRollDownToUpSeq0, Inverted::kLeftBarrelRollDownToUpSeq1, Inverted::kLeftBarrelRollDownToUpSeq2 }}, // LeftBarrelRollDownToUp
        {{ Inverted::kRightBarrelRollDownToUpSeq0, Inverted::kRightBarrelRollDownToUpSeq1, Inverted::kRightBarrelRollDownToUpSeq2 }}, // RightBarrelRollDownToUp
        {{ Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq0, Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq1, Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq2, Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq3 }}, // LeftBankToLeftQuarterTurn3TilesUp25
        {{ Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq0, Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq1, Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq2, Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq3 }}, // RightBankToRightQuarterTurn3TilesUp25
        {{ Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq1, Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 }}, // LeftQuarterTurn3TilesDown25ToLeftBank
        {{ Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq0, Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq1, Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq2, Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq3 }}, // RightQuarterTurn3TilesDown25ToRightBank
        {{ Inverted::kPoweredLiftSeq0 }}, // PoweredLift
        {{ Inverted::kLeftLargeHalfLoopUpSeq0, Inverted::kLeftLargeHalfLoopUpSeq1, Inverted::kLeftLargeHalfLoopUpSeq2, Inverted::kLeftLargeHalfLoopUpSeq3, Inverted::kLeftLargeHalfLoopUpSeq4, Inverted::kLeftLargeHalfLoopUpSeq5, Inverted::kLeftLargeHalfLoopUpSeq6 }}, // LeftLargeHalfLoopUp
        {{ Inverted::kRightLargeHalfLoopUpSeq0, Inverted::kRightLargeHalfLoopUpSeq1, Inverted::kRightLargeHalfLoopUpSeq2, Inverted::kRightLargeHalfLoopUpSeq3, Inverted::kRightLargeHalfLoopUpSeq4, Inverted::kRightLargeHalfLoopUpSeq5, Inverted::kRightLargeHalfLoopUpSeq6 }}, // RightLargeHalfLoopUp
        {{ Inverted::kLeftLargeHalfLoopDownSeq0, Inverted::kLeftLargeHalfLoopDownSeq1, Inverted::kLeftLargeHalfLoopDownSeq2, Inverted::kLeftLargeHalfLoopDownSeq3, Inverted::kLeftLargeHalfLoopDownSeq4, Inverted::kLeftLargeHalfLoopDownSeq5, Inverted::kLeftLargeHalfLoopDownSeq6 }}, // LeftLargeHalfLoopDown
        {{ Inverted::kRightLargeHalfLoopDownSeq0, Inverted::kRightLargeHalfLoopDownSeq1, Inverted::kRightLargeHalfLoopDownSeq2, Inverted::kRightLargeHalfLoopDownSeq3, Inverted::kRightLargeHalfLoopDownSeq4, Inverted::kRightLargeHalfLoopDownSeq5, Inverted::kRightLargeHalfLoopDownSeq6 }}, // RightLargeHalfLoopDown
        {{ Inverted::kLeftFlyerTwistUpSeq0, Inverted::kLeftFlyerTwistUpSeq1, Inverted::kLeftFlyerTwistUpSeq2 }}, // LeftFlyerTwistUp
        {{ Inverted::kRightFlyerTwistUpSeq0, Inverted::kRightFlyerTwistUpSeq1, Inverted::kRightFlyerTwistUpSeq2 }}, // RightFlyerTwistUp
        {{ Inverted::kLeftFlyerTwistDownSeq0, Inverted::kLeftFlyerTwistDownSeq1, Inverted::kLeftFlyerTwistDownSeq2 }}, // LeftFlyerTwistDown
        {{ Inverted::kRightFlyerTwistDownSeq0, Inverted::kRightFlyerTwistDownSeq1, Inverted::kRightFlyerTwistDownSeq2 }}, // RightFlyerTwistDown
        {{ Inverted::kFlyerHalfLoopUninvertedUpSeq0, Inverted::kFlyerHalfLoopUninvertedUpSeq1, Inverted::kFlyerHalfLoopUninvertedUpSeq2, Inverted::kFlyerHalfLoopUninvertedUpSeq3 }}, // FlyerHalfLoopUninvertedUp
        {{ Inverted::kFlyerHalfLoopInvertedDownSeq0, Inverted::kFlyerHalfLoopInvertedDownSeq1, Inverted::kFlyerHalfLoopInvertedDownSeq2, Inverted::kFlyerHalfLoopInvertedDownSeq3 }}, // FlyerHalfLoopInvertedDown
        {{ Inverted::kLeftFlyerCorkscrewUpSeq0, Inverted::kLeftFlyerCorkscrewUpSeq1, Inverted::kLeftFlyerCorkscrewUpSeq2 }}, // LeftFlyerCorkscrewUp
        {{ Inverted::kRightFlyerCorkscrewUpSeq0, Inverted::kRightFlyerCorkscrewUpSeq1, Inverted::kRightFlyerCorkscrewUpSeq2 }}, // RightFlyerCorkscrewUp
        {{ Inverted::kLeftFlyerCorkscrewDownSeq0, Inverted::kLeftFlyerCorkscrewDownSeq1, Inverted::kLeftFlyerCorkscrewDownSeq2 }}, // LeftFlyerCorkscrewDown
        {{ Inverted::kRightFlyerCorkscrewDownSeq0, Inverted::kRightFlyerCorkscrewDownSeq1, Inverted::kRightFlyerCorkscrewDownSeq2 }}, // RightFlyerCorkscrewDown
        {{ Inverted::kHeartLineTransferUpSeq0, Inverted::kHeartLineTransferUpSeq1, Inverted::kHeartLineTransferUpSeq2, Inverted::kHeartLineTransferUpSeq3 }}, // HeartLineTransferUp
        {{ Inverted::kHeartLineTransferDownSeq0, Inverted::kHeartLineTransferDownSeq1, Inverted::kHeartLineTransferDownSeq2, Inverted::kHeartLineTransferDownSeq3 }}, // HeartLineTransferDown
        {{ Inverted::kLeftHeartLineRollSeq0, Inverted::kLeftHeartLineRollSeq1, Inverted::kLeftHeartLineRollSeq2, Inverted::kLeftHeartLineRollSeq3, Inverted::kLeftHeartLineRollSeq4, Inverted::kLeftHeartLineRollSeq5 }}, // LeftHeartLineRoll
        {{ Inverted::kRightHeartLineRollSeq0, Inverted::kRightHeartLineRollSeq1, Inverted::kRightHeartLineRollSeq2, Inverted::kRightHeartLineRollSeq3, Inverted::kRightHeartLineRollSeq4, Inverted::kRightHeartLineRollSeq5 }}, // RightHeartLineRoll
        {{ Inverted::kMinigolfHoleASeq0, Inverted::kMinigolfHoleASeq1 }}, // MinigolfHoleA
        {{ Inverted::kMinigolfHoleBSeq0, Inverted::kMinigolfHoleBSeq1 }}, // MinigolfHoleB
        {{ Inverted::kMinigolfHoleCSeq0, Inverted::kMinigolfHoleCSeq1 }}, // MinigolfHoleC
        {{ Inverted::kMinigolfHoleDSeq0, Inverted::kMinigolfHoleDSeq1, Inverted::kMinigolfHoleDSeq2 }}, // MinigolfHoleD
        {{ Inverted::kMinigolfHoleESeq0, Inverted::kMinigolfHoleESeq1, Inverted::kMinigolfHoleESeq2 }}, // MinigolfHoleE
        {{ Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq0, Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq1, Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq2, Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq3 }}, // MultiDimInvertedFlatToDown90QuarterLoop
        {{ Inverted::kUp90ToInvertedFlatQuarterLoopSeq0, Inverted::kUp90ToInvertedFlatQuarterLoopSeq1, Inverted::kUp90ToInvertedFlatQuarterLoopSeq2 }}, // Up90ToInvertedFlatQuarterLoop
        {{ Inverted::kInvertedFlatToDown90QuarterLoopSeq0, Inverted::kInvertedFlatToDown90QuarterLoopSeq1, Inverted::kInvertedFlatToDown90QuarterLoopSeq2, Inverted::kInvertedFlatToDown90QuarterLoopSeq3 }}, // InvertedFlatToDown90QuarterLoop
        {{ Inverted::kLeftCurvedLiftHillSeq0, Inverted::kLeftCurvedLiftHillSeq1, Inverted::kLeftCurvedLiftHillSeq2, Inverted::kLeftCurvedLiftHillSeq3 }}, // LeftCurvedLiftHill
        {{ Inverted::kRightCurvedLiftHillSeq0, Inverted::kRightCurvedLiftHillSeq1, Inverted::kRightCurvedLiftHillSeq2, Inverted::kRightCurvedLiftHillSeq3 }}, // RightCurvedLiftHill
        {{ Inverted::kLeftReverserSeq0, Inverted::kLeftReverserSeq1, Inverted::kLeftReverserSeq2, Inverted::kLeftReverserSeq3, Inverted::kLeftReverserSeq4, Inverted::kLeftReverserSeq5 }}, // LeftReverser
        {{ Inverted::kRightReverserSeq0, Inverted::kRightReverserSeq1, Inverted::kRightReverserSeq2, Inverted::kRightReverserSeq3, Inverted::kRightReverserSeq4, Inverted::kRightReverserSeq5 }}, // RightReverser
        {{ Inverted::kAirThrustTopCapSeq0, Inverted::kAirThrustTopCapSeq1, Inverted::kAirThrustTopCapSeq2, Inverted::kAirThrustTopCapSeq3 }}, // AirThrustTopCap
        {{ Inverted::kAirThrustVerticalDownSeq0, Inverted::kAirThrustVerticalDownSeq1 }}, // AirThrustVerticalDown
        {{ Inverted::kAirThrustVerticalDownToLevelSeq0, Inverted::kAirThrustVerticalDownToLevelSeq1, Inverted::kAirThrustVerticalDownToLevelSeq2, Inverted::kAirThrustVerticalDownToLevelSeq3, Inverted::kAirThrustVerticalDownToLevelSeq4, Inverted::kAirThrustVerticalDownToLevelSeq5, Inverted::kAirThrustVerticalDownToLevelSeq6 }}, // AirThrustVerticalDownToLevel
        {{ Inverted::kBlockBrakesSeq0 }}, // BlockBrakes
        {{ Inverted::kLeftBankedQuarterTurn3TileUp25Seq0, Inverted::kLeftBankedQuarterTurn3TileUp25Seq1, Inverted::kLeftBankedQuarterTurn3TileUp25Seq2, Inverted::kLeftBankedQuarterTurn3TileUp25Seq3 }}, // LeftBankedQuarterTurn3TileUp25
        {{ Inverted::kRightBankedQuarterTurn3TileUp25Seq0, Inverted::kRightBankedQuarterTurn3TileUp25Seq1, Inverted::kRightBankedQuarterTurn3TileUp25Seq2, Inverted::kRightBankedQuarterTurn3TileUp25Seq3 }}, // RightBankedQuarterTurn3TileUp25
        {{ Inverted::kLeftBankedQuarterTurn3TileDown25Seq0, Inverted::kLeftBankedQuarterTurn3TileDown25Seq1, Inverted::kLeftBankedQuarterTurn3TileDown25Seq2, Inverted::kLeftBankedQuarterTurn3TileDown25Seq3 }}, // LeftBankedQuarterTurn3TileDown25
        {{ Inverted::kRightBankedQuarterTurn3TileDown25Seq0, Inverted::kRightBankedQuarterTurn3TileDown25Seq1, Inverted::kRightBankedQuarterTurn3TileDown25Seq2, Inverted::kRightBankedQuarterTurn3TileDown25Seq3 }}, // RightBankedQuarterTurn3TileDown25
        {{ Inverted::kLeftBankedQuarterTurn5TileUp25Seq0, Inverted::kLeftBankedQuarterTurn5TileUp25Seq1, Inverted::kLeftBankedQuarterTurn5TileUp25Seq2, Inverted::kLeftBankedQuarterTurn5TileUp25Seq3, Inverted::kLeftBankedQuarterTurn5TileUp25Seq4, Inverted::kLeftBankedQuarterTurn5TileUp25Seq5, Inverted::kLeftBankedQuarterTurn5TileUp25Seq6 }}, // LeftBankedQuarterTurn5TileUp25
        {{ Inverted::kRightBankedQuarterTurn5TileUp25Seq0, Inverted::kRightBankedQuarterTurn5TileUp25Seq1, Inverted::kRightBankedQuarterTurn5TileUp25Seq2, Inverted::kRightBankedQuarterTurn5TileUp25Seq3, Inverted::kRightBankedQuarterTurn5TileUp25Seq4, Inverted::kRightBankedQuarterTurn5TileUp25Seq5, Inverted::kRightBankedQuarterTurn5TileUp25Seq6 }}, // RightBankedQuarterTurn5TileUp25
        {{ Inverted::kLeftBankedQuarterTurn5TileDown25Seq0, Inverted::kLeftBankedQuarterTurn5TileDown25Seq1, Inverted::kLeftBankedQuarterTurn5TileDown25Seq2, Inverted::kLeftBankedQuarterTurn5TileDown25Seq3, Inverted::kLeftBankedQuarterTurn5TileDown25Seq4, Inverted::kLeftBankedQuarterTurn5TileDown25Seq5, Inverted::kLeftBankedQuarterTurn5TileDown25Seq6 }}, // LeftBankedQuarterTurn5TileDown25
        {{ Inverted::kRightBankedQuarterTurn5TileDown25Seq0, Inverted::kRightBankedQuarterTurn5TileDown25Seq1, Inverted::kRightBankedQuarterTurn5TileDown25Seq2, Inverted::kRightBankedQuarterTurn5TileDown25Seq3, Inverted::kRightBankedQuarterTurn5TileDown25Seq4, Inverted::kRightBankedQuarterTurn5TileDown25Seq5, Inverted::kRightBankedQuarterTurn5TileDown25Seq6 }}, // RightBankedQuarterTurn5TileDown25
        {{ Inverted::kUp25ToLeftBankedUp25Seq0 }}, // Up25ToLeftBankedUp25
        {{ Inverted::kUp25ToRightBankedUp25Seq0 }}, // Up25ToRightBankedUp25
        {{ Inverted::kLeftBankedUp25ToUp25Seq0 }}, // LeftBankedUp25ToUp25
        {{ Inverted::kRightBankedUp25ToUp25Seq0 }}, // RightBankedUp25ToUp25
        {{ Inverted::kDown25ToLeftBankedDown25Seq0 }}, // Down25ToLeftBankedDown25
        {{ Inverted::kDown25ToRightBankedDown25Seq0 }}, // Down25ToRightBankedDown25
        {{ Inverted::kLeftBankedDown25ToDown25Seq0 }}, // LeftBankedDown25ToDown25
        {{ Inverted::kRightBankedDown25ToDown25Seq0 }}, // RightBankedDown25ToDown25
        {{ Inverted::kLeftBankedFlatToLeftBankedUp25Seq0 }}, // LeftBankedFlatToLeftBankedUp25
        {{ Inverted::kRightBankedFlatToRightBankedUp25Seq0 }}, // RightBankedFlatToRightBankedUp25
        {{ Inverted::kLeftBankedUp25ToLeftBankedFlatSeq0 }}, // LeftBankedUp25ToLeftBankedFlat
        {{ Inverted::kRightBankedUp25ToRightBankedFlatSeq0 }}, // RightBankedUp25ToRightBankedFlat
        {{ Inverted::kLeftBankedFlatToLeftBankedDown25Seq0 }}, // LeftBankedFlatToLeftBankedDown25
        {{ Inverted::kRightBankedFlatToRightBankedDown25Seq0 }}, // RightBankedFlatToRightBankedDown25
        {{ Inverted::kLeftBankedDown25ToLeftBankedFlatSeq0 }}, // LeftBankedDown25ToLeftBankedFlat
        {{ Inverted::kRightBankedDown25ToRightBankedFlatSeq0 }}, // RightBankedDown25ToRightBankedFlat
        {{ Inverted::kFlatToLeftBankedUp25Seq0 }}, // FlatToLeftBankedUp25
        {{ Inverted::kFlatToRightBankedUp25Seq0 }}, // FlatToRightBankedUp25
        {{ Inverted::kLeftBankedUp25ToFlatSeq0 }}, // LeftBankedUp25ToFlat
        {{ Inverted::kRightBankedUp25ToFlatSeq0 }}, // RightBankedUp25ToFlat
        {{ Inverted::kFlatToLeftBankedDown25Seq0 }}, // FlatToLeftBankedDown25
        {{ Inverted::kFlatToRightBankedDown25Seq0 }}, // FlatToRightBankedDown25
        {{ Inverted::kLeftBankedDown25ToFlatSeq0 }}, // LeftBankedDown25ToFlat
        {{ Inverted::kRightBankedDown25ToFlatSeq0 }}, // RightBankedDown25ToFlat
        {{ Inverted::kLeftQuarterTurn1TileUp90Seq0, Inverted::kLeftQuarterTurn1TileUp90Seq1 }}, // LeftQuarterTurn1TileUp90
        {{ Inverted::kRightQuarterTurn1TileUp90Seq0, Inverted::kRightQuarterTurn1TileUp90Seq1 }}, // RightQuarterTurn1TileUp90
        {{ Inverted::kLeftQuarterTurn1TileDown90Seq0, Inverted::kLeftQuarterTurn1TileDown90Seq1 }}, // LeftQuarterTurn1TileDown90
        {{ Inverted::kRightQuarterTurn1TileDown90Seq0, Inverted::kRightQuarterTurn1TileDown90Seq1 }}, // RightQuarterTurn1TileDown90
        {{ Inverted::kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, Inverted::kMultiDimUp90ToInvertedFlatQuarterLoopSeq1, Inverted::kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 }}, // MultiDimUp90ToInvertedFlatQuarterLoop
        {{ Inverted::kMultiDimFlatToDown90QuarterLoopSeq0, Inverted::kMultiDimFlatToDown90QuarterLoopSeq1, Inverted::kMultiDimFlatToDown90QuarterLoopSeq2, Inverted::kMultiDimFlatToDown90QuarterLoopSeq3 }}, // MultiDimFlatToDown90QuarterLoop
        {{ Inverted::kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, Inverted::kMultiDimInvertedUp90ToFlatQuarterLoopSeq1, Inverted::kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 }}, // MultiDimInvertedUp90ToFlatQuarterLoop
        {{ Inverted::kRotationControlToggleSeq0 }}, // RotationControlToggle
        {{ Inverted::kFlatTrack1x4ASeq0, Inverted::kFlatTrack1x4ASeq1, Inverted::kFlatTrack1x4ASeq2, Inverted::kFlatTrack1x4ASeq3 }}, // FlatTrack1x4A
        {{ Inverted::kFlatTrack2x2Seq0, Inverted::kFlatTrack2x2Seq1, Inverted::kFlatTrack2x2Seq2, Inverted::kFlatTrack2x2Seq3 }}, // FlatTrack2x2
        {{ Inverted::kFlatTrack4x4Seq0, Inverted::kFlatTrack4x4Seq1, Inverted::kFlatTrack4x4Seq2, Inverted::kFlatTrack4x4Seq3, Inverted::kFlatTrack4x4Seq4, Inverted::kFlatTrack4x4Seq5, Inverted::kFlatTrack4x4Seq6, Inverted::kFlatTrack4x4Seq7, Inverted::kFlatTrack4x4Seq8, Inverted::kFlatTrack4x4Seq9, Inverted::kFlatTrack4x4Seq10, Inverted::kFlatTrack4x4Seq11, Inverted::kFlatTrack4x4Seq12, Inverted::kFlatTrack4x4Seq13, Inverted::kFlatTrack4x4Seq14, Inverted::kFlatTrack4x4Seq15 }}, // FlatTrack4x4
        {{ Inverted::kFlatTrack2x4Seq0, Inverted::kFlatTrack2x4Seq1, Inverted::kFlatTrack2x4Seq2, Inverted::kFlatTrack2x4Seq3, Inverted::kFlatTrack2x4Seq4, Inverted::kFlatTrack2x4Seq5, Inverted::kFlatTrack2x4Seq6, Inverted::kFlatTrack2x4Seq7 }}, // FlatTrack2x4
        {{ Inverted::kFlatTrack1x5Seq0, Inverted::kFlatTrack1x5Seq1, Inverted::kFlatTrack1x5Seq2, Inverted::kFlatTrack1x5Seq3, Inverted::kFlatTrack1x5Seq4 }}, // FlatTrack1x5
        {{ Inverted::kFlatTrack1x1ASeq0 }}, // FlatTrack1x1A
        {{ Inverted::kFlatTrack1x4BSeq0, Inverted::kFlatTrack1x4BSeq1, Inverted::kFlatTrack1x4BSeq2, Inverted::kFlatTrack1x4BSeq3 }}, // FlatTrack1x4B
        {{ Inverted::kFlatTrack1x1BSeq0 }}, // FlatTrack1x1B
        {{ Inverted::kFlatTrack1x4CSeq0, Inverted::kFlatTrack1x4CSeq1, Inverted::kFlatTrack1x4CSeq2, Inverted::kFlatTrack1x4CSeq3 }}, // FlatTrack1x4C
        {{ Inverted::kFlatTrack3x3Seq0, Inverted::kFlatTrack3x3Seq1, Inverted::kFlatTrack3x3Seq2, Inverted::kFlatTrack3x3Seq3, Inverted::kFlatTrack3x3Seq4, Inverted::kFlatTrack3x3Seq5, Inverted::kFlatTrack3x3Seq6, Inverted::kFlatTrack3x3Seq7, Inverted::kFlatTrack3x3Seq8 }}, // FlatTrack3x3
        {{ Inverted::kLeftLargeCorkscrewUpSeq0, Inverted::kLeftLargeCorkscrewUpSeq1, Inverted::kLeftLargeCorkscrewUpSeq2, Inverted::kLeftLargeCorkscrewUpSeq3, Inverted::kLeftLargeCorkscrewUpSeq4, Inverted::kLeftLargeCorkscrewUpSeq5 }}, // LeftLargeCorkscrewUp
        {{ Inverted::kRightLargeCorkscrewUpSeq0, Inverted::kRightLargeCorkscrewUpSeq1, Inverted::kRightLargeCorkscrewUpSeq2, Inverted::kRightLargeCorkscrewUpSeq3, Inverted::kRightLargeCorkscrewUpSeq4, Inverted::kRightLargeCorkscrewUpSeq5 }}, // RightLargeCorkscrewUp
        {{ Inverted::kLeftLargeCorkscrewDownSeq0, Inverted::kLeftLargeCorkscrewDownSeq1, Inverted::kLeftLargeCorkscrewDownSeq2, Inverted::kLeftLargeCorkscrewDownSeq3, Inverted::kLeftLargeCorkscrewDownSeq4, Inverted::kLeftLargeCorkscrewDownSeq5 }}, // LeftLargeCorkscrewDown
        {{ Inverted::kRightLargeCorkscrewDownSeq0, Inverted::kRightLargeCorkscrewDownSeq1, Inverted::kRightLargeCorkscrewDownSeq2, Inverted::kRightLargeCorkscrewDownSeq3, Inverted::kRightLargeCorkscrewDownSeq4, Inverted::kRightLargeCorkscrewDownSeq5 }}, // RightLargeCorkscrewDown
        {{ Inverted::kLeftMediumHalfLoopUpSeq0, Inverted::kLeftMediumHalfLoopUpSeq1, Inverted::kLeftMediumHalfLoopUpSeq2, Inverted::kLeftMediumHalfLoopUpSeq3, Inverted::kLeftMediumHalfLoopUpSeq4 }}, // LeftMediumHalfLoopUp
        {{ Inverted::kRightMediumHalfLoopUpSeq0, Inverted::kRightMediumHalfLoopUpSeq1, Inverted::kRightMediumHalfLoopUpSeq2, Inverted::kRightMediumHalfLoopUpSeq3, Inverted::kRightMediumHalfLoopUpSeq4 }}, // RightMediumHalfLoopUp
        {{ Inverted::kLeftMediumHalfLoopDownSeq0, Inverted::kLeftMediumHalfLoopDownSeq1, Inverted::kLeftMediumHalfLoopDownSeq2, Inverted::kLeftMediumHalfLoopDownSeq3, Inverted::kLeftMediumHalfLoopDownSeq4 }}, // LeftMediumHalfLoopDown
        {{ Inverted::kRightMediumHalfLoopDownSeq0, Inverted::kRightMediumHalfLoopDownSeq1, Inverted::kRightMediumHalfLoopDownSeq2, Inverted::kRightMediumHalfLoopDownSeq3, Inverted::kRightMediumHalfLoopDownSeq4 }}, // RightMediumHalfLoopDown
        {{ Inverted::kLeftZeroGRollUpSeq0, Inverted::kLeftZeroGRollUpSeq1, Inverted::kLeftZeroGRollUpSeq2 }}, // LeftZeroGRollUp
        {{ Inverted::kRightZeroGRollUpSeq0, Inverted::kRightZeroGRollUpSeq1, Inverted::kRightZeroGRollUpSeq2 }}, // RightZeroGRollUp
        {{ Inverted::kLeftZeroGRollDownSeq0, Inverted::kLeftZeroGRollDownSeq1, Inverted::kLeftZeroGRollDownSeq2 }}, // LeftZeroGRollDown
        {{ Inverted::kRightZeroGRollDownSeq0, Inverted::kRightZeroGRollDownSeq1, Inverted::kRightZeroGRollDownSeq2 }}, // RightZeroGRollDown
        {{ Inverted::kLeftLargeZeroGRollUpSeq0, Inverted::kLeftLargeZeroGRollUpSeq1, Inverted::kLeftLargeZeroGRollUpSeq2, Inverted::kLeftLargeZeroGRollUpSeq3 }}, // LeftLargeZeroGRollUp
        {{ Inverted::kRightLargeZeroGRollUpSeq0, Inverted::kRightLargeZeroGRollUpSeq1, Inverted::kRightLargeZeroGRollUpSeq2, Inverted::kRightLargeZeroGRollUpSeq3 }}, // RightLargeZeroGRollUp
        {{ Inverted::kLeftLargeZeroGRollDownSeq0, Inverted::kLeftLargeZeroGRollDownSeq1, Inverted::kLeftLargeZeroGRollDownSeq2, Inverted::kLeftLargeZeroGRollDownSeq3 }}, // LeftLargeZeroGRollDown
        {{ Inverted::kRightLargeZeroGRollDownSeq0, Inverted::kRightLargeZeroGRollDownSeq1, Inverted::kRightLargeZeroGRollDownSeq2, Inverted::kRightLargeZeroGRollDownSeq3 }}, // RightLargeZeroGRollDown
        {{ Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq0, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq1, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq2, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq3, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq4, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq5, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedUp
        {{ Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq0, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq1, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq2, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq3, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq4, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq5, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq6 }}, // RightFlyerLargeHalfLoopUninvertedUp
        {{ Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq0, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq1, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq2, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq3, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq4, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq5, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopInvertedDown
        {{ Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq0, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq1, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq2, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq3, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq4, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq5, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq6 }}, // RightFlyerLargeHalfLoopInvertedDown
        {{ Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq0, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq1, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq2, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq3, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq4, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq5, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopInvertedUp
        {{ Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq0, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq1, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq2, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq3, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq4, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq5, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq6 }}, // RightFlyerLargeHalfLoopInvertedUp
        {{ Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq0, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq1, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq2, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq3, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq4, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq5, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedDown
        {{ Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq0, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq1, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq2, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq3, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq4, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq5, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq6 }}, // RightFlyerLargeHalfLoopUninvertedDown
        {{ Inverted::kFlyerHalfLoopInvertedUpSeq0, Inverted::kFlyerHalfLoopInvertedUpSeq1, Inverted::kFlyerHalfLoopInvertedUpSeq2, Inverted::kFlyerHalfLoopInvertedUpSeq3 }}, // FlyerHalfLoopInvertedUp
        {{ Inverted::kFlyerHalfLoopUninvertedDownSeq0, Inverted::kFlyerHalfLoopUninvertedDownSeq1, Inverted::kFlyerHalfLoopUninvertedDownSeq2, Inverted::kFlyerHalfLoopUninvertedDownSeq3 }}, // FlyerHalfLoopUninvertedDown
        {{ Inverted::kLeftEighthToDiagUp25Seq0, Inverted::kLeftEighthToDiagUp25Seq1, Inverted::kLeftEighthToDiagUp25Seq2, Inverted::kLeftEighthToDiagUp25Seq3, Inverted::kLeftEighthToDiagUp25Seq4 }}, // LeftEighthToDiagUp25
        {{ Inverted::kRightEighthToDiagUp25Seq0, Inverted::kRightEighthToDiagUp25Seq1, Inverted::kRightEighthToDiagUp25Seq2, Inverted::kRightEighthToDiagUp25Seq3, Inverted::kRightEighthToDiagUp25Seq4 }}, // RightEighthToDiagUp25
        {{ Inverted::kLeftEighthToDiagDown25Seq0, Inverted::kLeftEighthToDiagDown25Seq1, Inverted::kLeftEighthToDiagDown25Seq2, Inverted::kLeftEighthToDiagDown25Seq3, Inverted::kLeftEighthToDiagDown25Seq4 }}, // LeftEighthToDiagDown25
        {{ Inverted::kRightEighthToDiagDown25Seq0, Inverted::kRightEighthToDiagDown25Seq1, Inverted::kRightEighthToDiagDown25Seq2, Inverted::kRightEighthToDiagDown25Seq3, Inverted::kRightEighthToDiagDown25Seq4 }}, // RightEighthToDiagDown25
        {{ Inverted::kLeftEighthToOrthogonalUp25Seq0, Inverted::kLeftEighthToOrthogonalUp25Seq1, Inverted::kLeftEighthToOrthogonalUp25Seq2, Inverted::kLeftEighthToOrthogonalUp25Seq3, Inverted::kLeftEighthToOrthogonalUp25Seq4 }}, // LeftEighthToOrthogonalUp25
        {{ Inverted::kRightEighthToOrthogonalUp25Seq0, Inverted::kRightEighthToOrthogonalUp25Seq1, Inverted::kRightEighthToOrthogonalUp25Seq2, Inverted::kRightEighthToOrthogonalUp25Seq3, Inverted::kRightEighthToOrthogonalUp25Seq4 }}, // RightEighthToOrthogonalUp25
        {{ Inverted::kLeftEighthToOrthogonalDown25Seq0, Inverted::kLeftEighthToOrthogonalDown25Seq1, Inverted::kLeftEighthToOrthogonalDown25Seq2, Inverted::kLeftEighthToOrthogonalDown25Seq3, Inverted::kLeftEighthToOrthogonalDown25Seq4 }}, // LeftEighthToOrthogonalDown25
        {{ Inverted::kRightEighthToOrthogonalDown25Seq0, Inverted::kRightEighthToOrthogonalDown25Seq1, Inverted::kRightEighthToOrthogonalDown25Seq2, Inverted::kRightEighthToOrthogonalDown25Seq3, Inverted::kRightEighthToOrthogonalDown25Seq4 }}, // RightEighthToOrthogonalDown25
        {{ Inverted::kDiagUp25ToLeftBankedUp25Seq0, Inverted::kDiagUp25ToLeftBankedUp25Seq1, Inverted::kDiagUp25ToLeftBankedUp25Seq2, Inverted::kDiagUp25ToLeftBankedUp25Seq3 }}, // DiagUp25ToLeftBankedUp25
        {{ Inverted::kDiagUp25ToRightBankedUp25Seq0, Inverted::kDiagUp25ToRightBankedUp25Seq1, Inverted::kDiagUp25ToRightBankedUp25Seq2, Inverted::kDiagUp25ToRightBankedUp25Seq3 }}, // DiagUp25ToRightBankedUp25
        {{ Inverted::kDiagLeftBankedUp25ToUp25Seq0, Inverted::kDiagLeftBankedUp25ToUp25Seq1, Inverted::kDiagLeftBankedUp25ToUp25Seq2, Inverted::kDiagLeftBankedUp25ToUp25Seq3 }}, // DiagLeftBankedUp25ToUp25
        {{ Inverted::kDiagRightBankedUp25ToUp25Seq0, Inverted::kDiagRightBankedUp25ToUp25Seq1, Inverted::kDiagRightBankedUp25ToUp25Seq2, Inverted::kDiagRightBankedUp25ToUp25Seq3 }}, // DiagRightBankedUp25ToUp25
        {{ Inverted::kDiagDown25ToLeftBankedDown25Seq0, Inverted::kDiagDown25ToLeftBankedDown25Seq1, Inverted::kDiagDown25ToLeftBankedDown25Seq2, Inverted::kDiagDown25ToLeftBankedDown25Seq3 }}, // DiagDown25ToLeftBankedDown25
        {{ Inverted::kDiagDown25ToRightBankedDown25Seq0, Inverted::kDiagDown25ToRightBankedDown25Seq1, Inverted::kDiagDown25ToRightBankedDown25Seq2, Inverted::kDiagDown25ToRightBankedDown25Seq3 }}, // DiagDown25ToRightBankedDown25
        {{ Inverted::kDiagLeftBankedDown25ToDown25Seq0, Inverted::kDiagLeftBankedDown25ToDown25Seq1, Inverted::kDiagLeftBankedDown25ToDown25Seq2, Inverted::kDiagLeftBankedDown25ToDown25Seq3 }}, // DiagLeftBankedDown25ToDown25
        {{ Inverted::kDiagRightBankedDown25ToDown25Seq0, Inverted::kDiagRightBankedDown25ToDown25Seq1, Inverted::kDiagRightBankedDown25ToDown25Seq2, Inverted::kDiagRightBankedDown25ToDown25Seq3 }}, // DiagRightBankedDown25ToDown25
        {{ Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq0, Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq1, Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq2, Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq3 }}, // DiagLeftBankedFlatToLeftBankedUp25
        {{ Inverted::kDiagRightBankedFlatToRightBankedUp25Seq0, Inverted::kDiagRightBankedFlatToRightBankedUp25Seq1, Inverted::kDiagRightBankedFlatToRightBankedUp25Seq2, Inverted::kDiagRightBankedFlatToRightBankedUp25Seq3 }}, // DiagRightBankedFlatToRightBankedUp25
        {{ Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq0, Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq1, Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq2, Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedUp25ToLeftBankedFlat
        {{ Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq0, Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq1, Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq2, Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq3 }}, // DiagRightBankedUp25ToRightBankedFlat
        {{ Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq0, Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq1, Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq2, Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq3 }}, // DiagLeftBankedFlatToLeftBankedDown25
        {{ Inverted::kDiagRightBankedFlatToRightBankedDown25Seq0, Inverted::kDiagRightBankedFlatToRightBankedDown25Seq1, Inverted::kDiagRightBankedFlatToRightBankedDown25Seq2, Inverted::kDiagRightBankedFlatToRightBankedDown25Seq3 }}, // DiagRightBankedFlatToRightBankedDown25
        {{ Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq0, Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq1, Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq2, Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedDown25ToLeftBankedFlat
        {{ Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq0, Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq1, Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq2, Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq3 }}, // DiagRightBankedDown25ToRightBankedFlat
        {{ Inverted::kDiagFlatToLeftBankedUp25Seq0, Inverted::kDiagFlatToLeftBankedUp25Seq1, Inverted::kDiagFlatToLeftBankedUp25Seq2, Inverted::kDiagFlatToLeftBankedUp25Seq3 }}, // DiagFlatToLeftBankedUp25
        {{ Inverted::kDiagFlatToRightBankedUp25Seq0, Inverted::kDiagFlatToRightBankedUp25Seq1, Inverted::kDiagFlatToRightBankedUp25Seq2, Inverted::kDiagFlatToRightBankedUp25Seq3 }}, // DiagFlatToRightBankedUp25
        {{ Inverted::kDiagLeftBankedUp25ToFlatSeq0, Inverted::kDiagLeftBankedUp25ToFlatSeq1, Inverted::kDiagLeftBankedUp25ToFlatSeq2, Inverted::kDiagLeftBankedUp25ToFlatSeq3 }}, // DiagLeftBankedUp25ToFlat
        {{ Inverted::kDiagRightBankedUp25ToFlatSeq0, Inverted::kDiagRightBankedUp25ToFlatSeq1, Inverted::kDiagRightBankedUp25ToFlatSeq2, Inverted::kDiagRightBankedUp25ToFlatSeq3 }}, // DiagRightBankedUp25ToFlat
        {{ Inverted::kDiagFlatToLeftBankedDown25Seq0, Inverted::kDiagFlatToLeftBankedDown25Seq1, Inverted::kDiagFlatToLeftBankedDown25Seq2, Inverted::kDiagFlatToLeftBankedDown25Seq3 }}, // DiagFlatToLeftBankedDown25
        {{ Inverted::kDiagFlatToRightBankedDown25Seq0, Inverted::kDiagFlatToRightBankedDown25Seq1, Inverted::kDiagFlatToRightBankedDown25Seq2, Inverted::kDiagFlatToRightBankedDown25Seq3 }}, // DiagFlatToRightBankedDown25
        {{ Inverted::kDiagLeftBankedDown25ToFlatSeq0, Inverted::kDiagLeftBankedDown25ToFlatSeq1, Inverted::kDiagLeftBankedDown25ToFlatSeq2, Inverted::kDiagLeftBankedDown25ToFlatSeq3 }}, // DiagLeftBankedDown25ToFlat
        {{ Inverted::kDiagRightBankedDown25ToFlatSeq0, Inverted::kDiagRightBankedDown25ToFlatSeq1, Inverted::kDiagRightBankedDown25ToFlatSeq2, Inverted::kDiagRightBankedDown25ToFlatSeq3 }}, // DiagRightBankedDown25ToFlat
        {{ Inverted::kDiagUp25LeftBankedSeq0, Inverted::kDiagUp25LeftBankedSeq1, Inverted::kDiagUp25LeftBankedSeq2, Inverted::kDiagUp25LeftBankedSeq3 }}, // DiagUp25LeftBanked
        {{ Inverted::kDiagUp25RightBankedSeq0, Inverted::kDiagUp25RightBankedSeq1, Inverted::kDiagUp25RightBankedSeq2, Inverted::kDiagUp25RightBankedSeq3 }}, // DiagUp25RightBanked
        {{ Inverted::kDiagDown25LeftBankedSeq0, Inverted::kDiagDown25LeftBankedSeq1, Inverted::kDiagDown25LeftBankedSeq2, Inverted::kDiagDown25LeftBankedSeq3 }}, // DiagDown25LeftBanked
        {{ Inverted::kDiagDown25RightBankedSeq0, Inverted::kDiagDown25RightBankedSeq1, Inverted::kDiagDown25RightBankedSeq2, Inverted::kDiagDown25RightBankedSeq3 }}, // DiagDown25RightBanked
        {{ Inverted::kLeftEighthBankToDiagUp25Seq0, Inverted::kLeftEighthBankToDiagUp25Seq1, Inverted::kLeftEighthBankToDiagUp25Seq2, Inverted::kLeftEighthBankToDiagUp25Seq3, Inverted::kLeftEighthBankToDiagUp25Seq4 }}, // LeftEighthBankToDiagUp25
        {{ Inverted::kRightEighthBankToDiagUp25Seq0, Inverted::kRightEighthBankToDiagUp25Seq1, Inverted::kRightEighthBankToDiagUp25Seq2, Inverted::kRightEighthBankToDiagUp25Seq3, Inverted::kRightEighthBankToDiagUp25Seq4 }}, // RightEighthBankToDiagUp25
        {{ Inverted::kLeftEighthBankToDiagDown25Seq0, Inverted::kLeftEighthBankToDiagDown25Seq1, Inverted::kLeftEighthBankToDiagDown25Seq2, Inverted::kLeftEighthBankToDiagDown25Seq3, Inverted::kLeftEighthBankToDiagDown25Seq4 }}, // LeftEighthBankToDiagDown25
        {{ Inverted::kRightEighthBankToDiagDown25Seq0, Inverted::kRightEighthBankToDiagDown25Seq1, Inverted::kRightEighthBankToDiagDown25Seq2, Inverted::kRightEighthBankToDiagDown25Seq3, Inverted::kRightEighthBankToDiagDown25Seq4 }}, // RightEighthBankToDiagDown25
        {{ Inverted::kLeftEighthBankToOrthogonalUp25Seq0, Inverted::kLeftEighthBankToOrthogonalUp25Seq1, Inverted::kLeftEighthBankToOrthogonalUp25Seq2, Inverted::kLeftEighthBankToOrthogonalUp25Seq3, Inverted::kLeftEighthBankToOrthogonalUp25Seq4 }}, // LeftEighthBankToOrthogonalUp25
        {{ Inverted::kRightEighthBankToOrthogonalUp25Seq0, Inverted::kRightEighthBankToOrthogonalUp25Seq1, Inverted::kRightEighthBankToOrthogonalUp25Seq2, Inverted::kRightEighthBankToOrthogonalUp25Seq3, Inverted::kRightEighthBankToOrthogonalUp25Seq4 }}, // RightEighthBankToOrthogonalUp25
        {{ Inverted::kLeftEighthBankToOrthogonalDown25Seq0, Inverted::kLeftEighthBankToOrthogonalDown25Seq1, Inverted::kLeftEighthBankToOrthogonalDown25Seq2, Inverted::kLeftEighthBankToOrthogonalDown25Seq3, Inverted::kLeftEighthBankToOrthogonalDown25Seq4 }}, // LeftEighthBankToOrthogonalDown25
        {{ Inverted::kRightEighthBankToOrthogonalDown25Seq0, Inverted::kRightEighthBankToOrthogonalDown25Seq1, Inverted::kRightEighthBankToOrthogonalDown25Seq2, Inverted::kRightEighthBankToOrthogonalDown25Seq3, Inverted::kRightEighthBankToOrthogonalDown25Seq4 }}, // RightEighthBankToOrthogonalDown25
        {{ Inverted::kDiagBrakesSeq0, Inverted::kDiagBrakesSeq1, Inverted::kDiagBrakesSeq2, Inverted::kDiagBrakesSeq3 }}, // DiagBrakes
        {{ Inverted::kDiagBlockBrakesSeq0, Inverted::kDiagBlockBrakesSeq1, Inverted::kDiagBlockBrakesSeq2, Inverted::kDiagBlockBrakesSeq3 }}, // DiagBlockBrakes
        {{ Inverted::kDown25BrakesSeq0 }}, // Down25Brakes
        {{ Inverted::kDiagBoosterSeq0, Inverted::kDiagBoosterSeq1, Inverted::kDiagBoosterSeq2, Inverted::kDiagBoosterSeq3 }}, // DiagBooster
        {{ Inverted::kDiagFlatToUp60LongBaseSeq0, Inverted::kDiagFlatToUp60LongBaseSeq1, Inverted::kDiagFlatToUp60LongBaseSeq2, Inverted::kDiagFlatToUp60LongBaseSeq3, Inverted::kDiagFlatToUp60LongBaseSeq4, Inverted::kDiagFlatToUp60LongBaseSeq5, Inverted::kDiagFlatToUp60LongBaseSeq6, Inverted::kDiagFlatToUp60LongBaseSeq7, Inverted::kDiagFlatToUp60LongBaseSeq8, Inverted::kDiagFlatToUp60LongBaseSeq9 }}, // DiagFlatToUp60LongBase
        {{ Inverted::kDiagUp60ToFlatLongBaseSeq0, Inverted::kDiagUp60ToFlatLongBaseSeq1, Inverted::kDiagUp60ToFlatLongBaseSeq2, Inverted::kDiagUp60ToFlatLongBaseSeq3, Inverted::kDiagUp60ToFlatLongBaseSeq4, Inverted::kDiagUp60ToFlatLongBaseSeq5, Inverted::kDiagUp60ToFlatLongBaseSeq6, Inverted::kDiagUp60ToFlatLongBaseSeq7, Inverted::kDiagUp60ToFlatLongBaseSeq8, Inverted::kDiagUp60ToFlatLongBaseSeq9 }}, // DiagUp60ToFlatLongBase
        {{ Inverted::kDiagFlatToDown60LongBaseSeq0, Inverted::kDiagFlatToDown60LongBaseSeq1, Inverted::kDiagFlatToDown60LongBaseSeq2, Inverted::kDiagFlatToDown60LongBaseSeq3, Inverted::kDiagFlatToDown60LongBaseSeq4, Inverted::kDiagFlatToDown60LongBaseSeq5, Inverted::kDiagFlatToDown60LongBaseSeq6, Inverted::kDiagFlatToDown60LongBaseSeq7, Inverted::kDiagFlatToDown60LongBaseSeq8, Inverted::kDiagFlatToDown60LongBaseSeq9 }}, // DiagFlatToDown60LongBase
        {{ Inverted::kDiagDown60ToFlatLongBaseSeq0, Inverted::kDiagDown60ToFlatLongBaseSeq1, Inverted::kDiagDown60ToFlatLongBaseSeq2, Inverted::kDiagDown60ToFlatLongBaseSeq3, Inverted::kDiagDown60ToFlatLongBaseSeq4, Inverted::kDiagDown60ToFlatLongBaseSeq5, Inverted::kDiagDown60ToFlatLongBaseSeq6, Inverted::kDiagDown60ToFlatLongBaseSeq7, Inverted::kDiagDown60ToFlatLongBaseSeq8, Inverted::kDiagDown60ToFlatLongBaseSeq9 }}, // DiagDown60ToFlatLongBase
        {{ Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq0, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq1, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq2, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq3, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq4, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq5 }}, // LeftEighthDiveLoopUpToOrthogonal
        {{ Inverted::kRightEighthDiveLoopUpToOrthogonalSeq0, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq1, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq2, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq3, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq4, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq5 }}, // RightEighthDiveLoopUpToOrthogonal
        {{ Inverted::kLeftEighthDiveLoopDownToDiagSeq0, Inverted::kLeftEighthDiveLoopDownToDiagSeq1, Inverted::kLeftEighthDiveLoopDownToDiagSeq2, Inverted::kLeftEighthDiveLoopDownToDiagSeq3, Inverted::kLeftEighthDiveLoopDownToDiagSeq4, Inverted::kLeftEighthDiveLoopDownToDiagSeq5 }}, // LeftEighthDiveLoopDownToDiag
        {{ Inverted::kRightEighthDiveLoopDownToDiagSeq0, Inverted::kRightEighthDiveLoopDownToDiagSeq1, Inverted::kRightEighthDiveLoopDownToDiagSeq2, Inverted::kRightEighthDiveLoopDownToDiagSeq3, Inverted::kRightEighthDiveLoopDownToDiagSeq4, Inverted::kRightEighthDiveLoopDownToDiagSeq5 }}, // RightEighthDiveLoopDownToDiag
        {{ Inverted::kDiagDown25BrakesSeq0, Inverted::kDiagDown25BrakesSeq1, Inverted::kDiagDown25BrakesSeq2, Inverted::kDiagDown25BrakesSeq3 }}, // DiagDown25Brakes
    };

    static_assert(std::size(kBlockedSegmentsInverted) == EnumValue(TrackElemType::Count));

    static constexpr const std::array<uint16_t, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> kBlockedSegmentsWide[] = {
        {{ Wide::kFlatSeq0 }}, // Flat
        {{ Wide::kEndStationSeq0 }}, // EndStation
        {{ Wide::kBeginStationSeq0 }}, // BeginStation
        {{ Wide::kMiddleStationSeq0 }}, // MiddleStation
        {{ Wide::kUp25Seq0 }}, // Up25
        {{ Wide::kUp60Seq0 }}, // Up60
        {{ Wide::kFlatToUp25Seq0 }}, // FlatToUp25
        {{ Wide::kUp25ToUp60Seq0 }}, // Up25ToUp60
        {{ Wide::kUp60ToUp25Seq0 }}, // Up60ToUp25
        {{ Wide::kUp25ToFlatSeq0 }}, // Up25ToFlat
        {{ Wide::kDown25Seq0 }}, // Down25
        {{ Wide::kDown60Seq0 }}, // Down60
        {{ Wide::kFlatToDown25Seq0 }}, // FlatToDown25
        {{ Wide::kDown25ToDown60Seq0 }}, // Down25ToDown60
        {{ Wide::kDown60ToDown25Seq0 }}, // Down60ToDown25
        {{ Wide::kDown25ToFlatSeq0 }}, // Down25ToFlat
        {{ Wide::kLeftQuarterTurn5TilesSeq0, Wide::kLeftQuarterTurn5TilesSeq1, Wide::kLeftQuarterTurn5TilesSeq2, Wide::kLeftQuarterTurn5TilesSeq3, Wide::kLeftQuarterTurn5TilesSeq4, Wide::kLeftQuarterTurn5TilesSeq5, Wide::kLeftQuarterTurn5TilesSeq6 }}, // LeftQuarterTurn5Tiles
        {{ Wide::kRightQuarterTurn5TilesSeq0, Wide::kRightQuarterTurn5TilesSeq1, Wide::kRightQuarterTurn5TilesSeq2, Wide::kRightQuarterTurn5TilesSeq3, Wide::kRightQuarterTurn5TilesSeq4, Wide::kRightQuarterTurn5TilesSeq5, Wide::kRightQuarterTurn5TilesSeq6 }}, // RightQuarterTurn5Tiles
        {{ Wide::kFlatToLeftBankSeq0 }}, // FlatToLeftBank
        {{ Wide::kFlatToRightBankSeq0 }}, // FlatToRightBank
        {{ Wide::kLeftBankToFlatSeq0 }}, // LeftBankToFlat
        {{ Wide::kRightBankToFlatSeq0 }}, // RightBankToFlat
        {{ Wide::kBankedLeftQuarterTurn5TilesSeq0, Wide::kBankedLeftQuarterTurn5TilesSeq1, Wide::kBankedLeftQuarterTurn5TilesSeq2, Wide::kBankedLeftQuarterTurn5TilesSeq3, Wide::kBankedLeftQuarterTurn5TilesSeq4, Wide::kBankedLeftQuarterTurn5TilesSeq5, Wide::kBankedLeftQuarterTurn5TilesSeq6 }}, // BankedLeftQuarterTurn5Tiles
        {{ Wide::kBankedRightQuarterTurn5TilesSeq0, Wide::kBankedRightQuarterTurn5TilesSeq1, Wide::kBankedRightQuarterTurn5TilesSeq2, Wide::kBankedRightQuarterTurn5TilesSeq3, Wide::kBankedRightQuarterTurn5TilesSeq4, Wide::kBankedRightQuarterTurn5TilesSeq5, Wide::kBankedRightQuarterTurn5TilesSeq6 }}, // BankedRightQuarterTurn5Tiles
        {{ Wide::kLeftBankToUp25Seq0 }}, // LeftBankToUp25
        {{ Wide::kRightBankToUp25Seq0 }}, // RightBankToUp25
        {{ Wide::kUp25ToLeftBankSeq0 }}, // Up25ToLeftBank
        {{ Wide::kUp25ToRightBankSeq0 }}, // Up25ToRightBank
        {{ Wide::kLeftBankToDown25Seq0 }}, // LeftBankToDown25
        {{ Wide::kRightBankToDown25Seq0 }}, // RightBankToDown25
        {{ Wide::kDown25ToLeftBankSeq0 }}, // Down25ToLeftBank
        {{ Wide::kDown25ToRightBankSeq0 }}, // Down25ToRightBank
        {{ Wide::kLeftBankSeq0 }}, // LeftBank
        {{ Wide::kRightBankSeq0 }}, // RightBank
        {{ Wide::kLeftQuarterTurn5TilesUp25Seq0, Wide::kLeftQuarterTurn5TilesUp25Seq1, Wide::kLeftQuarterTurn5TilesUp25Seq2, Wide::kLeftQuarterTurn5TilesUp25Seq3, Wide::kLeftQuarterTurn5TilesUp25Seq4, Wide::kLeftQuarterTurn5TilesUp25Seq5, Wide::kLeftQuarterTurn5TilesUp25Seq6 }}, // LeftQuarterTurn5TilesUp25
        {{ Wide::kRightQuarterTurn5TilesUp25Seq0, Wide::kRightQuarterTurn5TilesUp25Seq1, Wide::kRightQuarterTurn5TilesUp25Seq2, Wide::kRightQuarterTurn5TilesUp25Seq3, Wide::kRightQuarterTurn5TilesUp25Seq4, Wide::kRightQuarterTurn5TilesUp25Seq5, Wide::kRightQuarterTurn5TilesUp25Seq6 }}, // RightQuarterTurn5TilesUp25
        {{ Wide::kLeftQuarterTurn5TilesDown25Seq0, Wide::kLeftQuarterTurn5TilesDown25Seq1, Wide::kLeftQuarterTurn5TilesDown25Seq2, Wide::kLeftQuarterTurn5TilesDown25Seq3, Wide::kLeftQuarterTurn5TilesDown25Seq4, Wide::kLeftQuarterTurn5TilesDown25Seq5, Wide::kLeftQuarterTurn5TilesDown25Seq6 }}, // LeftQuarterTurn5TilesDown25
        {{ Wide::kRightQuarterTurn5TilesDown25Seq0, Wide::kRightQuarterTurn5TilesDown25Seq1, Wide::kRightQuarterTurn5TilesDown25Seq2, Wide::kRightQuarterTurn5TilesDown25Seq3, Wide::kRightQuarterTurn5TilesDown25Seq4, Wide::kRightQuarterTurn5TilesDown25Seq5, Wide::kRightQuarterTurn5TilesDown25Seq6 }}, // RightQuarterTurn5TilesDown25
        {{ Wide::kSBendLeftSeq0, Wide::kSBendLeftSeq1, Wide::kSBendLeftSeq2, Wide::kSBendLeftSeq3 }}, // SBendLeft
        {{ Wide::kSBendRightSeq0, Wide::kSBendRightSeq1, Wide::kSBendRightSeq2, Wide::kSBendRightSeq3 }}, // SBendRight
        {{ Wide::kLeftVerticalLoopSeq0, Wide::kLeftVerticalLoopSeq1, Wide::kLeftVerticalLoopSeq2, Wide::kLeftVerticalLoopSeq3, Wide::kLeftVerticalLoopSeq4, Wide::kLeftVerticalLoopSeq5, Wide::kLeftVerticalLoopSeq6, Wide::kLeftVerticalLoopSeq7, Wide::kLeftVerticalLoopSeq8, Wide::kLeftVerticalLoopSeq9 }}, // LeftVerticalLoop
        {{ Wide::kRightVerticalLoopSeq0, Wide::kRightVerticalLoopSeq1, Wide::kRightVerticalLoopSeq2, Wide::kRightVerticalLoopSeq3, Wide::kRightVerticalLoopSeq4, Wide::kRightVerticalLoopSeq5, Wide::kRightVerticalLoopSeq6, Wide::kRightVerticalLoopSeq7, Wide::kRightVerticalLoopSeq8, Wide::kRightVerticalLoopSeq9 }}, // RightVerticalLoop
        {{ Wide::kLeftQuarterTurn3TilesSeq0, Wide::kLeftQuarterTurn3TilesSeq1, Wide::kLeftQuarterTurn3TilesSeq2, Wide::kLeftQuarterTurn3TilesSeq3 }}, // LeftQuarterTurn3Tiles
        {{ Wide::kRightQuarterTurn3TilesSeq0, Wide::kRightQuarterTurn3TilesSeq1, Wide::kRightQuarterTurn3TilesSeq2, Wide::kRightQuarterTurn3TilesSeq3 }}, // RightQuarterTurn3Tiles
        {{ Wide::kLeftBankedQuarterTurn3TilesSeq0, Wide::kLeftBankedQuarterTurn3TilesSeq1, Wide::kLeftBankedQuarterTurn3TilesSeq2, Wide::kLeftBankedQuarterTurn3TilesSeq3 }}, // LeftBankedQuarterTurn3Tiles
        {{ Wide::kRightBankedQuarterTurn3TilesSeq0, Wide::kRightBankedQuarterTurn3TilesSeq1, Wide::kRightBankedQuarterTurn3TilesSeq2, Wide::kRightBankedQuarterTurn3TilesSeq3 }}, // RightBankedQuarterTurn3Tiles
        {{ Wide::kLeftQuarterTurn3TilesUp25Seq0, Wide::kLeftQuarterTurn3TilesUp25Seq1, Wide::kLeftQuarterTurn3TilesUp25Seq2, Wide::kLeftQuarterTurn3TilesUp25Seq3 }}, // LeftQuarterTurn3TilesUp25
        {{ Wide::kRightQuarterTurn3TilesUp25Seq0, Wide::kRightQuarterTurn3TilesUp25Seq1, Wide::kRightQuarterTurn3TilesUp25Seq2, Wide::kRightQuarterTurn3TilesUp25Seq3 }}, // RightQuarterTurn3TilesUp25
        {{ Wide::kLeftQuarterTurn3TilesDown25Seq0, Wide::kLeftQuarterTurn3TilesDown25Seq1, Wide::kLeftQuarterTurn3TilesDown25Seq2, Wide::kLeftQuarterTurn3TilesDown25Seq3 }}, // LeftQuarterTurn3TilesDown25
        {{ Wide::kRightQuarterTurn3TilesDown25Seq0, Wide::kRightQuarterTurn3TilesDown25Seq1, Wide::kRightQuarterTurn3TilesDown25Seq2, Wide::kRightQuarterTurn3TilesDown25Seq3 }}, // RightQuarterTurn3TilesDown25
        {{ Wide::kLeftQuarterTurn1TileSeq0 }}, // LeftQuarterTurn1Tile
        {{ Wide::kRightQuarterTurn1TileSeq0 }}, // RightQuarterTurn1Tile
        {{ Wide::kLeftTwistDownToUpSeq0, Wide::kLeftTwistDownToUpSeq1, Wide::kLeftTwistDownToUpSeq2 }}, // LeftTwistDownToUp
        {{ Wide::kRightTwistDownToUpSeq0, Wide::kRightTwistDownToUpSeq1, Wide::kRightTwistDownToUpSeq2 }}, // RightTwistDownToUp
        {{ Wide::kLeftTwistUpToDownSeq0, Wide::kLeftTwistUpToDownSeq1, Wide::kLeftTwistUpToDownSeq2 }}, // LeftTwistUpToDown
        {{ Wide::kRightTwistUpToDownSeq0, Wide::kRightTwistUpToDownSeq1, Wide::kRightTwistUpToDownSeq2 }}, // RightTwistUpToDown
        {{ Wide::kHalfLoopUpSeq0, Wide::kHalfLoopUpSeq1, Wide::kHalfLoopUpSeq2, Wide::kHalfLoopUpSeq3 }}, // HalfLoopUp
        {{ Wide::kHalfLoopDownSeq0, Wide::kHalfLoopDownSeq1, Wide::kHalfLoopDownSeq2, Wide::kHalfLoopDownSeq3 }}, // HalfLoopDown
        {{ Wide::kLeftCorkscrewUpSeq0, Wide::kLeftCorkscrewUpSeq1, Wide::kLeftCorkscrewUpSeq2 }}, // LeftCorkscrewUp
        {{ Wide::kRightCorkscrewUpSeq0, Wide::kRightCorkscrewUpSeq1, Wide::kRightCorkscrewUpSeq2 }}, // RightCorkscrewUp
        {{ Wide::kLeftCorkscrewDownSeq0, Wide::kLeftCorkscrewDownSeq1, Wide::kLeftCorkscrewDownSeq2 }}, // LeftCorkscrewDown
        {{ Wide::kRightCorkscrewDownSeq0, Wide::kRightCorkscrewDownSeq1, Wide::kRightCorkscrewDownSeq2 }}, // RightCorkscrewDown
        {{ Wide::kFlatToUp60Seq0 }}, // FlatToUp60
        {{ Wide::kUp60ToFlatSeq0 }}, // Up60ToFlat
        {{ Wide::kFlatToDown60Seq0 }}, // FlatToDown60
        {{ Wide::kDown60ToFlatSeq0 }}, // Down60ToFlat
        {{ Wide::kTowerBaseSeq0, Wide::kTowerBaseSeq1, Wide::kTowerBaseSeq2, Wide::kTowerBaseSeq3, Wide::kTowerBaseSeq4, Wide::kTowerBaseSeq5, Wide::kTowerBaseSeq6, Wide::kTowerBaseSeq7, Wide::kTowerBaseSeq8 }}, // TowerBase
        {{ Wide::kTowerSectionSeq0, Wide::kTowerSectionSeq1 }}, // TowerSection
        {{ Wide::kFlatCoveredSeq0 }}, // FlatCovered
        {{ Wide::kUp25CoveredSeq0 }}, // Up25Covered
        {{ Wide::kUp60CoveredSeq0 }}, // Up60Covered
        {{ Wide::kFlatToUp25CoveredSeq0 }}, // FlatToUp25Covered
        {{ Wide::kUp25ToUp60CoveredSeq0 }}, // Up25ToUp60Covered
        {{ Wide::kUp60ToUp25CoveredSeq0 }}, // Up60ToUp25Covered
        {{ Wide::kUp25ToFlatCoveredSeq0 }}, // Up25ToFlatCovered
        {{ Wide::kDown25CoveredSeq0 }}, // Down25Covered
        {{ Wide::kDown60CoveredSeq0 }}, // Down60Covered
        {{ Wide::kFlatToDown25CoveredSeq0 }}, // FlatToDown25Covered
        {{ Wide::kDown25ToDown60CoveredSeq0 }}, // Down25ToDown60Covered
        {{ Wide::kDown60ToDown25CoveredSeq0 }}, // Down60ToDown25Covered
        {{ Wide::kDown25ToFlatCoveredSeq0 }}, // Down25ToFlatCovered
        {{ Wide::kLeftQuarterTurn5TilesCoveredSeq0, Wide::kLeftQuarterTurn5TilesCoveredSeq1, Wide::kLeftQuarterTurn5TilesCoveredSeq2, Wide::kLeftQuarterTurn5TilesCoveredSeq3, Wide::kLeftQuarterTurn5TilesCoveredSeq4, Wide::kLeftQuarterTurn5TilesCoveredSeq5, Wide::kLeftQuarterTurn5TilesCoveredSeq6 }}, // LeftQuarterTurn5TilesCovered
        {{ Wide::kRightQuarterTurn5TilesCoveredSeq0, Wide::kRightQuarterTurn5TilesCoveredSeq1, Wide::kRightQuarterTurn5TilesCoveredSeq2, Wide::kRightQuarterTurn5TilesCoveredSeq3, Wide::kRightQuarterTurn5TilesCoveredSeq4, Wide::kRightQuarterTurn5TilesCoveredSeq5, Wide::kRightQuarterTurn5TilesCoveredSeq6 }}, // RightQuarterTurn5TilesCovered
        {{ Wide::kSBendLeftCoveredSeq0, Wide::kSBendLeftCoveredSeq1, Wide::kSBendLeftCoveredSeq2, Wide::kSBendLeftCoveredSeq3 }}, // SBendLeftCovered
        {{ Wide::kSBendRightCoveredSeq0, Wide::kSBendRightCoveredSeq1, Wide::kSBendRightCoveredSeq2, Wide::kSBendRightCoveredSeq3 }}, // SBendRightCovered
        {{ Wide::kLeftQuarterTurn3TilesCoveredSeq0, Wide::kLeftQuarterTurn3TilesCoveredSeq1, Wide::kLeftQuarterTurn3TilesCoveredSeq2, Wide::kLeftQuarterTurn3TilesCoveredSeq3 }}, // LeftQuarterTurn3TilesCovered
        {{ Wide::kRightQuarterTurn3TilesCoveredSeq0, Wide::kRightQuarterTurn3TilesCoveredSeq1, Wide::kRightQuarterTurn3TilesCoveredSeq2, Wide::kRightQuarterTurn3TilesCoveredSeq3 }}, // RightQuarterTurn3TilesCovered
        {{ Wide::kLeftHalfBankedHelixUpSmallSeq0, Wide::kLeftHalfBankedHelixUpSmallSeq1, Wide::kLeftHalfBankedHelixUpSmallSeq2, Wide::kLeftHalfBankedHelixUpSmallSeq3, Wide::kLeftHalfBankedHelixUpSmallSeq4, Wide::kLeftHalfBankedHelixUpSmallSeq5, Wide::kLeftHalfBankedHelixUpSmallSeq6, Wide::kLeftHalfBankedHelixUpSmallSeq7 }}, // LeftHalfBankedHelixUpSmall
        {{ Wide::kRightHalfBankedHelixUpSmallSeq0, Wide::kRightHalfBankedHelixUpSmallSeq1, Wide::kRightHalfBankedHelixUpSmallSeq2, Wide::kRightHalfBankedHelixUpSmallSeq3, Wide::kRightHalfBankedHelixUpSmallSeq4, Wide::kRightHalfBankedHelixUpSmallSeq5, Wide::kRightHalfBankedHelixUpSmallSeq6, Wide::kRightHalfBankedHelixUpSmallSeq7 }}, // RightHalfBankedHelixUpSmall
        {{ Wide::kLeftHalfBankedHelixDownSmallSeq0, Wide::kLeftHalfBankedHelixDownSmallSeq1, Wide::kLeftHalfBankedHelixDownSmallSeq2, Wide::kLeftHalfBankedHelixDownSmallSeq3, Wide::kLeftHalfBankedHelixDownSmallSeq4, Wide::kLeftHalfBankedHelixDownSmallSeq5, Wide::kLeftHalfBankedHelixDownSmallSeq6, Wide::kLeftHalfBankedHelixDownSmallSeq7 }}, // LeftHalfBankedHelixDownSmall
        {{ Wide::kRightHalfBankedHelixDownSmallSeq0, Wide::kRightHalfBankedHelixDownSmallSeq1, Wide::kRightHalfBankedHelixDownSmallSeq2, Wide::kRightHalfBankedHelixDownSmallSeq3, Wide::kRightHalfBankedHelixDownSmallSeq4, Wide::kRightHalfBankedHelixDownSmallSeq5, Wide::kRightHalfBankedHelixDownSmallSeq6, Wide::kRightHalfBankedHelixDownSmallSeq7 }}, // RightHalfBankedHelixDownSmall
        {{ Wide::kLeftHalfBankedHelixUpLargeSeq0, Wide::kLeftHalfBankedHelixUpLargeSeq1, Wide::kLeftHalfBankedHelixUpLargeSeq2, Wide::kLeftHalfBankedHelixUpLargeSeq3, Wide::kLeftHalfBankedHelixUpLargeSeq4, Wide::kLeftHalfBankedHelixUpLargeSeq5, Wide::kLeftHalfBankedHelixUpLargeSeq6, Wide::kLeftHalfBankedHelixUpLargeSeq7, Wide::kLeftHalfBankedHelixUpLargeSeq8, Wide::kLeftHalfBankedHelixUpLargeSeq9, Wide::kLeftHalfBankedHelixUpLargeSeq10, Wide::kLeftHalfBankedHelixUpLargeSeq11, Wide::kLeftHalfBankedHelixUpLargeSeq12, Wide::kLeftHalfBankedHelixUpLargeSeq13 }}, // LeftHalfBankedHelixUpLarge
        {{ Wide::kRightHalfBankedHelixUpLargeSeq0, Wide::kRightHalfBankedHelixUpLargeSeq1, Wide::kRightHalfBankedHelixUpLargeSeq2, Wide::kRightHalfBankedHelixUpLargeSeq3, Wide::kRightHalfBankedHelixUpLargeSeq4, Wide::kRightHalfBankedHelixUpLargeSeq5, Wide::kRightHalfBankedHelixUpLargeSeq6, Wide::kRightHalfBankedHelixUpLargeSeq7, Wide::kRightHalfBankedHelixUpLargeSeq8, Wide::kRightHalfBankedHelixUpLargeSeq9, Wide::kRightHalfBankedHelixUpLargeSeq10, Wide::kRightHalfBankedHelixUpLargeSeq11, Wide::kRightHalfBankedHelixUpLargeSeq12, Wide::kRightHalfBankedHelixUpLargeSeq13 }}, // RightHalfBankedHelixUpLarge
        {{ Wide::kLeftHalfBankedHelixDownLargeSeq0, Wide::kLeftHalfBankedHelixDownLargeSeq1, Wide::kLeftHalfBankedHelixDownLargeSeq2, Wide::kLeftHalfBankedHelixDownLargeSeq3, Wide::kLeftHalfBankedHelixDownLargeSeq4, Wide::kLeftHalfBankedHelixDownLargeSeq5, Wide::kLeftHalfBankedHelixDownLargeSeq6, Wide::kLeftHalfBankedHelixDownLargeSeq7, Wide::kLeftHalfBankedHelixDownLargeSeq8, Wide::kLeftHalfBankedHelixDownLargeSeq9, Wide::kLeftHalfBankedHelixDownLargeSeq10, Wide::kLeftHalfBankedHelixDownLargeSeq11, Wide::kLeftHalfBankedHelixDownLargeSeq12, Wide::kLeftHalfBankedHelixDownLargeSeq13 }}, // LeftHalfBankedHelixDownLarge
        {{ Wide::kRightHalfBankedHelixDownLargeSeq0, Wide::kRightHalfBankedHelixDownLargeSeq1, Wide::kRightHalfBankedHelixDownLargeSeq2, Wide::kRightHalfBankedHelixDownLargeSeq3, Wide::kRightHalfBankedHelixDownLargeSeq4, Wide::kRightHalfBankedHelixDownLargeSeq5, Wide::kRightHalfBankedHelixDownLargeSeq6, Wide::kRightHalfBankedHelixDownLargeSeq7, Wide::kRightHalfBankedHelixDownLargeSeq8, Wide::kRightHalfBankedHelixDownLargeSeq9, Wide::kRightHalfBankedHelixDownLargeSeq10, Wide::kRightHalfBankedHelixDownLargeSeq11, Wide::kRightHalfBankedHelixDownLargeSeq12, Wide::kRightHalfBankedHelixDownLargeSeq13 }}, // RightHalfBankedHelixDownLarge
        {{ Wide::kLeftQuarterTurn1TileUp60Seq0 }}, // LeftQuarterTurn1TileUp60
        {{ Wide::kRightQuarterTurn1TileUp60Seq0 }}, // RightQuarterTurn1TileUp60
        {{ Wide::kLeftQuarterTurn1TileDown60Seq0 }}, // LeftQuarterTurn1TileDown60
        {{ Wide::kRightQuarterTurn1TileDown60Seq0 }}, // RightQuarterTurn1TileDown60
        {{ Wide::kBrakesSeq0 }}, // Brakes
        {{ Wide::kBoosterSeq0 }}, // Booster
        {{ Wide::kMazeSeq0 }}, // Maze
        {{ Wide::kLeftQuarterBankedHelixLargeUpSeq0, Wide::kLeftQuarterBankedHelixLargeUpSeq1, Wide::kLeftQuarterBankedHelixLargeUpSeq2, Wide::kLeftQuarterBankedHelixLargeUpSeq3, Wide::kLeftQuarterBankedHelixLargeUpSeq4, Wide::kLeftQuarterBankedHelixLargeUpSeq5, Wide::kLeftQuarterBankedHelixLargeUpSeq6 }}, // LeftQuarterBankedHelixLargeUp
        {{ Wide::kRightQuarterBankedHelixLargeUpSeq0, Wide::kRightQuarterBankedHelixLargeUpSeq1, Wide::kRightQuarterBankedHelixLargeUpSeq2, Wide::kRightQuarterBankedHelixLargeUpSeq3, Wide::kRightQuarterBankedHelixLargeUpSeq4, Wide::kRightQuarterBankedHelixLargeUpSeq5, Wide::kRightQuarterBankedHelixLargeUpSeq6 }}, // RightQuarterBankedHelixLargeUp
        {{ Wide::kLeftQuarterBankedHelixLargeDownSeq0, Wide::kLeftQuarterBankedHelixLargeDownSeq1, Wide::kLeftQuarterBankedHelixLargeDownSeq2, Wide::kLeftQuarterBankedHelixLargeDownSeq3, Wide::kLeftQuarterBankedHelixLargeDownSeq4, Wide::kLeftQuarterBankedHelixLargeDownSeq5, Wide::kLeftQuarterBankedHelixLargeDownSeq6 }}, // LeftQuarterBankedHelixLargeDown
        {{ Wide::kRightQuarterBankedHelixLargeDownSeq0, Wide::kRightQuarterBankedHelixLargeDownSeq1, Wide::kRightQuarterBankedHelixLargeDownSeq2, Wide::kRightQuarterBankedHelixLargeDownSeq3, Wide::kRightQuarterBankedHelixLargeDownSeq4, Wide::kRightQuarterBankedHelixLargeDownSeq5, Wide::kRightQuarterBankedHelixLargeDownSeq6 }}, // RightQuarterBankedHelixLargeDown
        {{ Wide::kLeftQuarterHelixLargeUpSeq0, Wide::kLeftQuarterHelixLargeUpSeq1, Wide::kLeftQuarterHelixLargeUpSeq2, Wide::kLeftQuarterHelixLargeUpSeq3, Wide::kLeftQuarterHelixLargeUpSeq4, Wide::kLeftQuarterHelixLargeUpSeq5, Wide::kLeftQuarterHelixLargeUpSeq6 }}, // LeftQuarterHelixLargeUp
        {{ Wide::kRightQuarterHelixLargeUpSeq0, Wide::kRightQuarterHelixLargeUpSeq1, Wide::kRightQuarterHelixLargeUpSeq2, Wide::kRightQuarterHelixLargeUpSeq3, Wide::kRightQuarterHelixLargeUpSeq4, Wide::kRightQuarterHelixLargeUpSeq5, Wide::kRightQuarterHelixLargeUpSeq6 }}, // RightQuarterHelixLargeUp
        {{ Wide::kLeftQuarterHelixLargeDownSeq0, Wide::kLeftQuarterHelixLargeDownSeq1, Wide::kLeftQuarterHelixLargeDownSeq2, Wide::kLeftQuarterHelixLargeDownSeq3, Wide::kLeftQuarterHelixLargeDownSeq4, Wide::kLeftQuarterHelixLargeDownSeq5, Wide::kLeftQuarterHelixLargeDownSeq6 }}, // LeftQuarterHelixLargeDown
        {{ Wide::kRightQuarterHelixLargeDownSeq0, Wide::kRightQuarterHelixLargeDownSeq1, Wide::kRightQuarterHelixLargeDownSeq2, Wide::kRightQuarterHelixLargeDownSeq3, Wide::kRightQuarterHelixLargeDownSeq4, Wide::kRightQuarterHelixLargeDownSeq5, Wide::kRightQuarterHelixLargeDownSeq6 }}, // RightQuarterHelixLargeDown
        {{ Wide::kUp25LeftBankedSeq0 }}, // Up25LeftBanked
        {{ Wide::kUp25RightBankedSeq0 }}, // Up25RightBanked
        {{ Wide::kWaterfallSeq0 }}, // Waterfall
        {{ Wide::kRapidsSeq0 }}, // Rapids
        {{ Wide::kOnRidePhotoSeq0 }}, // OnRidePhoto
        {{ Wide::kDown25LeftBankedSeq0 }}, // Down25LeftBanked
        {{ Wide::kDown25RightBankedSeq0 }}, // Down25RightBanked
        {{ Wide::kWatersplashSeq0, Wide::kWatersplashSeq1, Wide::kWatersplashSeq2, Wide::kWatersplashSeq3, Wide::kWatersplashSeq4 }}, // Watersplash
        {{ Wide::kFlatToUp60LongBaseSeq0, Wide::kFlatToUp60LongBaseSeq1, Wide::kFlatToUp60LongBaseSeq2, Wide::kFlatToUp60LongBaseSeq3 }}, // FlatToUp60LongBase
        {{ Wide::kUp60ToFlatLongBaseSeq0, Wide::kUp60ToFlatLongBaseSeq1, Wide::kUp60ToFlatLongBaseSeq2, Wide::kUp60ToFlatLongBaseSeq3 }}, // Up60ToFlatLongBase
        {{ Wide::kWhirlpoolSeq0 }}, // Whirlpool
        {{ Wide::kDown60ToFlatLongBaseSeq0, Wide::kDown60ToFlatLongBaseSeq1, Wide::kDown60ToFlatLongBaseSeq2, Wide::kDown60ToFlatLongBaseSeq3 }}, // Down60ToFlatLongBase
        {{ Wide::kFlatToDown60LongBaseSeq0, Wide::kFlatToDown60LongBaseSeq1, Wide::kFlatToDown60LongBaseSeq2, Wide::kFlatToDown60LongBaseSeq3 }}, // FlatToDown60LongBase
        {{ Wide::kCableLiftHillSeq0, Wide::kCableLiftHillSeq1, Wide::kCableLiftHillSeq2, Wide::kCableLiftHillSeq3 }}, // CableLiftHill
        {{ Wide::kReverseFreefallSlopeSeq0, Wide::kReverseFreefallSlopeSeq1, Wide::kReverseFreefallSlopeSeq2, Wide::kReverseFreefallSlopeSeq3, Wide::kReverseFreefallSlopeSeq4, Wide::kReverseFreefallSlopeSeq5, Wide::kReverseFreefallSlopeSeq6 }}, // ReverseFreefallSlope
        {{ Wide::kReverseFreefallVerticalSeq0, Wide::kReverseFreefallVerticalSeq1 }}, // ReverseFreefallVertical
        {{ Wide::kUp90Seq0, Wide::kUp90Seq1 }}, // Up90
        {{ Wide::kDown90Seq0, Wide::kDown90Seq1 }}, // Down90
        {{ Wide::kUp60ToUp90Seq0, Wide::kUp60ToUp90Seq1 }}, // Up60ToUp90
        {{ Wide::kDown90ToDown60Seq0 }}, // Down90ToDown60
        {{ Wide::kUp90ToUp60Seq0 }}, // Up90ToUp60
        {{ Wide::kDown60ToDown90Seq0, Wide::kDown60ToDown90Seq1 }}, // Down60ToDown90
        {{ Wide::kBrakeForDropSeq0 }}, // BrakeForDrop
        {{ Wide::kLeftEighthToDiagSeq0, Wide::kLeftEighthToDiagSeq1, Wide::kLeftEighthToDiagSeq2, Wide::kLeftEighthToDiagSeq3, Wide::kLeftEighthToDiagSeq4 }}, // LeftEighthToDiag
        {{ Wide::kRightEighthToDiagSeq0, Wide::kRightEighthToDiagSeq1, Wide::kRightEighthToDiagSeq2, Wide::kRightEighthToDiagSeq3, Wide::kRightEighthToDiagSeq4 }}, // RightEighthToDiag
        {{ Wide::kLeftEighthToOrthogonalSeq0, Wide::kLeftEighthToOrthogonalSeq1, Wide::kLeftEighthToOrthogonalSeq2, Wide::kLeftEighthToOrthogonalSeq3, Wide::kLeftEighthToOrthogonalSeq4 }}, // LeftEighthToOrthogonal
        {{ Wide::kRightEighthToOrthogonalSeq0, Wide::kRightEighthToOrthogonalSeq1, Wide::kRightEighthToOrthogonalSeq2, Wide::kRightEighthToOrthogonalSeq3, Wide::kRightEighthToOrthogonalSeq4 }}, // RightEighthToOrthogonal
        {{ Wide::kLeftEighthBankToDiagSeq0, Wide::kLeftEighthBankToDiagSeq1, Wide::kLeftEighthBankToDiagSeq2, Wide::kLeftEighthBankToDiagSeq3, Wide::kLeftEighthBankToDiagSeq4 }}, // LeftEighthBankToDiag
        {{ Wide::kRightEighthBankToDiagSeq0, Wide::kRightEighthBankToDiagSeq1, Wide::kRightEighthBankToDiagSeq2, Wide::kRightEighthBankToDiagSeq3, Wide::kRightEighthBankToDiagSeq4 }}, // RightEighthBankToDiag
        {{ Wide::kLeftEighthBankToOrthogonalSeq0, Wide::kLeftEighthBankToOrthogonalSeq1, Wide::kLeftEighthBankToOrthogonalSeq2, Wide::kLeftEighthBankToOrthogonalSeq3, Wide::kLeftEighthBankToOrthogonalSeq4 }}, // LeftEighthBankToOrthogonal
        {{ Wide::kRightEighthBankToOrthogonalSeq0, Wide::kRightEighthBankToOrthogonalSeq1, Wide::kRightEighthBankToOrthogonalSeq2, Wide::kRightEighthBankToOrthogonalSeq3, Wide::kRightEighthBankToOrthogonalSeq4 }}, // RightEighthBankToOrthogonal
        {{ Wide::kDiagFlatSeq0, Wide::kDiagFlatSeq1, Wide::kDiagFlatSeq2, Wide::kDiagFlatSeq3 }}, // DiagFlat
        {{ Wide::kDiagUp25Seq0, Wide::kDiagUp25Seq1, Wide::kDiagUp25Seq2, Wide::kDiagUp25Seq3 }}, // DiagUp25
        {{ Wide::kDiagUp60Seq0, Wide::kDiagUp60Seq1, Wide::kDiagUp60Seq2, Wide::kDiagUp60Seq3 }}, // DiagUp60
        {{ Wide::kDiagFlatToUp25Seq0, Wide::kDiagFlatToUp25Seq1, Wide::kDiagFlatToUp25Seq2, Wide::kDiagFlatToUp25Seq3 }}, // DiagFlatToUp25
        {{ Wide::kDiagUp25ToUp60Seq0, Wide::kDiagUp25ToUp60Seq1, Wide::kDiagUp25ToUp60Seq2, Wide::kDiagUp25ToUp60Seq3 }}, // DiagUp25ToUp60
        {{ Wide::kDiagUp60ToUp25Seq0, Wide::kDiagUp60ToUp25Seq1, Wide::kDiagUp60ToUp25Seq2, Wide::kDiagUp60ToUp25Seq3 }}, // DiagUp60ToUp25
        {{ Wide::kDiagUp25ToFlatSeq0, Wide::kDiagUp25ToFlatSeq1, Wide::kDiagUp25ToFlatSeq2, Wide::kDiagUp25ToFlatSeq3 }}, // DiagUp25ToFlat
        {{ Wide::kDiagDown25Seq0, Wide::kDiagDown25Seq1, Wide::kDiagDown25Seq2, Wide::kDiagDown25Seq3 }}, // DiagDown25
        {{ Wide::kDiagDown60Seq0, Wide::kDiagDown60Seq1, Wide::kDiagDown60Seq2, Wide::kDiagDown60Seq3 }}, // DiagDown60
        {{ Wide::kDiagFlatToDown25Seq0, Wide::kDiagFlatToDown25Seq1, Wide::kDiagFlatToDown25Seq2, Wide::kDiagFlatToDown25Seq3 }}, // DiagFlatToDown25
        {{ Wide::kDiagDown25ToDown60Seq0, Wide::kDiagDown25ToDown60Seq1, Wide::kDiagDown25ToDown60Seq2, Wide::kDiagDown25ToDown60Seq3 }}, // DiagDown25ToDown60
        {{ Wide::kDiagDown60ToDown25Seq0, Wide::kDiagDown60ToDown25Seq1, Wide::kDiagDown60ToDown25Seq2, Wide::kDiagDown60ToDown25Seq3 }}, // DiagDown60ToDown25
        {{ Wide::kDiagDown25ToFlatSeq0, Wide::kDiagDown25ToFlatSeq1, Wide::kDiagDown25ToFlatSeq2, Wide::kDiagDown25ToFlatSeq3 }}, // DiagDown25ToFlat
        {{ Wide::kDiagFlatToUp60Seq0, Wide::kDiagFlatToUp60Seq1, Wide::kDiagFlatToUp60Seq2, Wide::kDiagFlatToUp60Seq3 }}, // DiagFlatToUp60
        {{ Wide::kDiagUp60ToFlatSeq0, Wide::kDiagUp60ToFlatSeq1, Wide::kDiagUp60ToFlatSeq2, Wide::kDiagUp60ToFlatSeq3 }}, // DiagUp60ToFlat
        {{ Wide::kDiagFlatToDown60Seq0, Wide::kDiagFlatToDown60Seq1, Wide::kDiagFlatToDown60Seq2, Wide::kDiagFlatToDown60Seq3 }}, // DiagFlatToDown60
        {{ Wide::kDiagDown60ToFlatSeq0, Wide::kDiagDown60ToFlatSeq1, Wide::kDiagDown60ToFlatSeq2, Wide::kDiagDown60ToFlatSeq3 }}, // DiagDown60ToFlat
        {{ Wide::kDiagFlatToLeftBankSeq0, Wide::kDiagFlatToLeftBankSeq1, Wide::kDiagFlatToLeftBankSeq2, Wide::kDiagFlatToLeftBankSeq3 }}, // DiagFlatToLeftBank
        {{ Wide::kDiagFlatToRightBankSeq0, Wide::kDiagFlatToRightBankSeq1, Wide::kDiagFlatToRightBankSeq2, Wide::kDiagFlatToRightBankSeq3 }}, // DiagFlatToRightBank
        {{ Wide::kDiagLeftBankToFlatSeq0, Wide::kDiagLeftBankToFlatSeq1, Wide::kDiagLeftBankToFlatSeq2, Wide::kDiagLeftBankToFlatSeq3 }}, // DiagLeftBankToFlat
        {{ Wide::kDiagRightBankToFlatSeq0, Wide::kDiagRightBankToFlatSeq1, Wide::kDiagRightBankToFlatSeq2, Wide::kDiagRightBankToFlatSeq3 }}, // DiagRightBankToFlat
        {{ Wide::kDiagLeftBankToUp25Seq0, Wide::kDiagLeftBankToUp25Seq1, Wide::kDiagLeftBankToUp25Seq2, Wide::kDiagLeftBankToUp25Seq3 }}, // DiagLeftBankToUp25
        {{ Wide::kDiagRightBankToUp25Seq0, Wide::kDiagRightBankToUp25Seq1, Wide::kDiagRightBankToUp25Seq2, Wide::kDiagRightBankToUp25Seq3 }}, // DiagRightBankToUp25
        {{ Wide::kDiagUp25ToLeftBankSeq0, Wide::kDiagUp25ToLeftBankSeq1, Wide::kDiagUp25ToLeftBankSeq2, Wide::kDiagUp25ToLeftBankSeq3 }}, // DiagUp25ToLeftBank
        {{ Wide::kDiagUp25ToRightBankSeq0, Wide::kDiagUp25ToRightBankSeq1, Wide::kDiagUp25ToRightBankSeq2, Wide::kDiagUp25ToRightBankSeq3 }}, // DiagUp25ToRightBank
        {{ Wide::kDiagLeftBankToDown25Seq0, Wide::kDiagLeftBankToDown25Seq1, Wide::kDiagLeftBankToDown25Seq2, Wide::kDiagLeftBankToDown25Seq3 }}, // DiagLeftBankToDown25
        {{ Wide::kDiagRightBankToDown25Seq0, Wide::kDiagRightBankToDown25Seq1, Wide::kDiagRightBankToDown25Seq2, Wide::kDiagRightBankToDown25Seq3 }}, // DiagRightBankToDown25
        {{ Wide::kDiagDown25ToLeftBankSeq0, Wide::kDiagDown25ToLeftBankSeq1, Wide::kDiagDown25ToLeftBankSeq2, Wide::kDiagDown25ToLeftBankSeq3 }}, // DiagDown25ToLeftBank
        {{ Wide::kDiagDown25ToRightBankSeq0, Wide::kDiagDown25ToRightBankSeq1, Wide::kDiagDown25ToRightBankSeq2, Wide::kDiagDown25ToRightBankSeq3 }}, // DiagDown25ToRightBank
        {{ Wide::kDiagLeftBankSeq0, Wide::kDiagLeftBankSeq1, Wide::kDiagLeftBankSeq2, Wide::kDiagLeftBankSeq3 }}, // DiagLeftBank
        {{ Wide::kDiagRightBankSeq0, Wide::kDiagRightBankSeq1, Wide::kDiagRightBankSeq2, Wide::kDiagRightBankSeq3 }}, // DiagRightBank
        {{ Wide::kLogFlumeReverserSeq0 }}, // LogFlumeReverser
        {{ Wide::kSpinningTunnelSeq0 }}, // SpinningTunnel
        {{ Wide::kLeftBarrelRollUpToDownSeq0, Wide::kLeftBarrelRollUpToDownSeq1, Wide::kLeftBarrelRollUpToDownSeq2 }}, // LeftBarrelRollUpToDown
        {{ Wide::kRightBarrelRollUpToDownSeq0, Wide::kRightBarrelRollUpToDownSeq1, Wide::kRightBarrelRollUpToDownSeq2 }}, // RightBarrelRollUpToDown
        {{ Wide::kLeftBarrelRollDownToUpSeq0, Wide::kLeftBarrelRollDownToUpSeq1, Wide::kLeftBarrelRollDownToUpSeq2 }}, // LeftBarrelRollDownToUp
        {{ Wide::kRightBarrelRollDownToUpSeq0, Wide::kRightBarrelRollDownToUpSeq1, Wide::kRightBarrelRollDownToUpSeq2 }}, // RightBarrelRollDownToUp
        {{ Wide::kLeftBankToLeftQuarterTurn3TilesUp25Seq0, Wide::kLeftBankToLeftQuarterTurn3TilesUp25Seq1, Wide::kLeftBankToLeftQuarterTurn3TilesUp25Seq2, Wide::kLeftBankToLeftQuarterTurn3TilesUp25Seq3 }}, // LeftBankToLeftQuarterTurn3TilesUp25
        {{ Wide::kRightBankToRightQuarterTurn3TilesUp25Seq0, Wide::kRightBankToRightQuarterTurn3TilesUp25Seq1, Wide::kRightBankToRightQuarterTurn3TilesUp25Seq2, Wide::kRightBankToRightQuarterTurn3TilesUp25Seq3 }}, // RightBankToRightQuarterTurn3TilesUp25
        {{ Wide::kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, Wide::kLeftQuarterTurn3TilesDown25ToLeftBankSeq1, Wide::kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, Wide::kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 }}, // LeftQuarterTurn3TilesDown25ToLeftBank
        {{ Wide::kRightQuarterTurn3TilesDown25ToRightBankSeq0, Wide::kRightQuarterTurn3TilesDown25ToRightBankSeq1, Wide::kRightQuarterTurn3TilesDown25ToRightBankSeq2, Wide::kRightQuarterTurn3TilesDown25ToRightBankSeq3 }}, // RightQuarterTurn3TilesDown25ToRightBank
        {{ Wide::kPoweredLiftSeq0 }}, // PoweredLift
        {{ Wide::kLeftLargeHalfLoopUpSeq0, Wide::kLeftLargeHalfLoopUpSeq1, Wide::kLeftLargeHalfLoopUpSeq2, Wide::kLeftLargeHalfLoopUpSeq3, Wide::kLeftLargeHalfLoopUpSeq4, Wide::kLeftLargeHalfLoopUpSeq5, Wide::kLeftLargeHalfLoopUpSeq6 }}, // LeftLargeHalfLoopUp
        {{ Wide::kRightLargeHalfLoopUpSeq0, Wide::kRightLargeHalfLoopUpSeq1, Wide::kRightLargeHalfLoopUpSeq2, Wide::kRightLargeHalfLoopUpSeq3, Wide::kRightLargeHalfLoopUpSeq4, Wide::kRightLargeHalfLoopUpSeq5, Wide::kRightLargeHalfLoopUpSeq6 }}, // RightLargeHalfLoopUp
        {{ Wide::kLeftLargeHalfLoopDownSeq0, Wide::kLeftLargeHalfLoopDownSeq1, Wide::kLeftLargeHalfLoopDownSeq2, Wide::kLeftLargeHalfLoopDownSeq3, Wide::kLeftLargeHalfLoopDownSeq4, Wide::kLeftLargeHalfLoopDownSeq5, Wide::kLeftLargeHalfLoopDownSeq6 }}, // LeftLargeHalfLoopDown
        {{ Wide::kRightLargeHalfLoopDownSeq0, Wide::kRightLargeHalfLoopDownSeq1, Wide::kRightLargeHalfLoopDownSeq2, Wide::kRightLargeHalfLoopDownSeq3, Wide::kRightLargeHalfLoopDownSeq4, Wide::kRightLargeHalfLoopDownSeq5, Wide::kRightLargeHalfLoopDownSeq6 }}, // RightLargeHalfLoopDown
        {{ Wide::kLeftFlyerTwistUpSeq0, Wide::kLeftFlyerTwistUpSeq1, Wide::kLeftFlyerTwistUpSeq2 }}, // LeftFlyerTwistUp
        {{ Wide::kRightFlyerTwistUpSeq0, Wide::kRightFlyerTwistUpSeq1, Wide::kRightFlyerTwistUpSeq2 }}, // RightFlyerTwistUp
        {{ Wide::kLeftFlyerTwistDownSeq0, Wide::kLeftFlyerTwistDownSeq1, Wide::kLeftFlyerTwistDownSeq2 }}, // LeftFlyerTwistDown
        {{ Wide::kRightFlyerTwistDownSeq0, Wide::kRightFlyerTwistDownSeq1, Wide::kRightFlyerTwistDownSeq2 }}, // RightFlyerTwistDown
        {{ Wide::kFlyerHalfLoopUninvertedUpSeq0, Wide::kFlyerHalfLoopUninvertedUpSeq1, Wide::kFlyerHalfLoopUninvertedUpSeq2, Wide::kFlyerHalfLoopUninvertedUpSeq3 }}, // FlyerHalfLoopUninvertedUp
        {{ Wide::kFlyerHalfLoopInvertedDownSeq0, Wide::kFlyerHalfLoopInvertedDownSeq1, Wide::kFlyerHalfLoopInvertedDownSeq2, Wide::kFlyerHalfLoopInvertedDownSeq3 }}, // FlyerHalfLoopInvertedDown
        {{ Wide::kLeftFlyerCorkscrewUpSeq0, Wide::kLeftFlyerCorkscrewUpSeq1, Wide::kLeftFlyerCorkscrewUpSeq2 }}, // LeftFlyerCorkscrewUp
        {{ Wide::kRightFlyerCorkscrewUpSeq0, Wide::kRightFlyerCorkscrewUpSeq1, Wide::kRightFlyerCorkscrewUpSeq2 }}, // RightFlyerCorkscrewUp
        {{ Wide::kLeftFlyerCorkscrewDownSeq0, Wide::kLeftFlyerCorkscrewDownSeq1, Wide::kLeftFlyerCorkscrewDownSeq2 }}, // LeftFlyerCorkscrewDown
        {{ Wide::kRightFlyerCorkscrewDownSeq0, Wide::kRightFlyerCorkscrewDownSeq1, Wide::kRightFlyerCorkscrewDownSeq2 }}, // RightFlyerCorkscrewDown
        {{ Wide::kHeartLineTransferUpSeq0, Wide::kHeartLineTransferUpSeq1, Wide::kHeartLineTransferUpSeq2, Wide::kHeartLineTransferUpSeq3 }}, // HeartLineTransferUp
        {{ Wide::kHeartLineTransferDownSeq0, Wide::kHeartLineTransferDownSeq1, Wide::kHeartLineTransferDownSeq2, Wide::kHeartLineTransferDownSeq3 }}, // HeartLineTransferDown
        {{ Wide::kLeftHeartLineRollSeq0, Wide::kLeftHeartLineRollSeq1, Wide::kLeftHeartLineRollSeq2, Wide::kLeftHeartLineRollSeq3, Wide::kLeftHeartLineRollSeq4, Wide::kLeftHeartLineRollSeq5 }}, // LeftHeartLineRoll
        {{ Wide::kRightHeartLineRollSeq0, Wide::kRightHeartLineRollSeq1, Wide::kRightHeartLineRollSeq2, Wide::kRightHeartLineRollSeq3, Wide::kRightHeartLineRollSeq4, Wide::kRightHeartLineRollSeq5 }}, // RightHeartLineRoll
        {{ Wide::kMinigolfHoleASeq0, Wide::kMinigolfHoleASeq1 }}, // MinigolfHoleA
        {{ Wide::kMinigolfHoleBSeq0, Wide::kMinigolfHoleBSeq1 }}, // MinigolfHoleB
        {{ Wide::kMinigolfHoleCSeq0, Wide::kMinigolfHoleCSeq1 }}, // MinigolfHoleC
        {{ Wide::kMinigolfHoleDSeq0, Wide::kMinigolfHoleDSeq1, Wide::kMinigolfHoleDSeq2 }}, // MinigolfHoleD
        {{ Wide::kMinigolfHoleESeq0, Wide::kMinigolfHoleESeq1, Wide::kMinigolfHoleESeq2 }}, // MinigolfHoleE
        {{ Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq0, Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq1, Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq2, Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq3 }}, // MultiDimInvertedFlatToDown90QuarterLoop
        {{ Wide::kUp90ToInvertedFlatQuarterLoopSeq0, Wide::kUp90ToInvertedFlatQuarterLoopSeq1, Wide::kUp90ToInvertedFlatQuarterLoopSeq2 }}, // Up90ToInvertedFlatQuarterLoop
        {{ Wide::kInvertedFlatToDown90QuarterLoopSeq0, Wide::kInvertedFlatToDown90QuarterLoopSeq1, Wide::kInvertedFlatToDown90QuarterLoopSeq2, Wide::kInvertedFlatToDown90QuarterLoopSeq3 }}, // InvertedFlatToDown90QuarterLoop
        {{ Wide::kLeftCurvedLiftHillSeq0, Wide::kLeftCurvedLiftHillSeq1, Wide::kLeftCurvedLiftHillSeq2, Wide::kLeftCurvedLiftHillSeq3 }}, // LeftCurvedLiftHill
        {{ Wide::kRightCurvedLiftHillSeq0, Wide::kRightCurvedLiftHillSeq1, Wide::kRightCurvedLiftHillSeq2, Wide::kRightCurvedLiftHillSeq3 }}, // RightCurvedLiftHill
        {{ Wide::kLeftReverserSeq0, Wide::kLeftReverserSeq1, Wide::kLeftReverserSeq2, Wide::kLeftReverserSeq3, Wide::kLeftReverserSeq4, Wide::kLeftReverserSeq5 }}, // LeftReverser
        {{ Wide::kRightReverserSeq0, Wide::kRightReverserSeq1, Wide::kRightReverserSeq2, Wide::kRightReverserSeq3, Wide::kRightReverserSeq4, Wide::kRightReverserSeq5 }}, // RightReverser
        {{ Wide::kAirThrustTopCapSeq0, Wide::kAirThrustTopCapSeq1, Wide::kAirThrustTopCapSeq2, Wide::kAirThrustTopCapSeq3 }}, // AirThrustTopCap
        {{ Wide::kAirThrustVerticalDownSeq0, Wide::kAirThrustVerticalDownSeq1 }}, // AirThrustVerticalDown
        {{ Wide::kAirThrustVerticalDownToLevelSeq0, Wide::kAirThrustVerticalDownToLevelSeq1, Wide::kAirThrustVerticalDownToLevelSeq2, Wide::kAirThrustVerticalDownToLevelSeq3, Wide::kAirThrustVerticalDownToLevelSeq4, Wide::kAirThrustVerticalDownToLevelSeq5, Wide::kAirThrustVerticalDownToLevelSeq6 }}, // AirThrustVerticalDownToLevel
        {{ Wide::kBlockBrakesSeq0 }}, // BlockBrakes
        {{ Wide::kLeftBankedQuarterTurn3TileUp25Seq0, Wide::kLeftBankedQuarterTurn3TileUp25Seq1, Wide::kLeftBankedQuarterTurn3TileUp25Seq2, Wide::kLeftBankedQuarterTurn3TileUp25Seq3 }}, // LeftBankedQuarterTurn3TileUp25
        {{ Wide::kRightBankedQuarterTurn3TileUp25Seq0, Wide::kRightBankedQuarterTurn3TileUp25Seq1, Wide::kRightBankedQuarterTurn3TileUp25Seq2, Wide::kRightBankedQuarterTurn3TileUp25Seq3 }}, // RightBankedQuarterTurn3TileUp25
        {{ Wide::kLeftBankedQuarterTurn3TileDown25Seq0, Wide::kLeftBankedQuarterTurn3TileDown25Seq1, Wide::kLeftBankedQuarterTurn3TileDown25Seq2, Wide::kLeftBankedQuarterTurn3TileDown25Seq3 }}, // LeftBankedQuarterTurn3TileDown25
        {{ Wide::kRightBankedQuarterTurn3TileDown25Seq0, Wide::kRightBankedQuarterTurn3TileDown25Seq1, Wide::kRightBankedQuarterTurn3TileDown25Seq2, Wide::kRightBankedQuarterTurn3TileDown25Seq3 }}, // RightBankedQuarterTurn3TileDown25
        {{ Wide::kLeftBankedQuarterTurn5TileUp25Seq0, Wide::kLeftBankedQuarterTurn5TileUp25Seq1, Wide::kLeftBankedQuarterTurn5TileUp25Seq2, Wide::kLeftBankedQuarterTurn5TileUp25Seq3, Wide::kLeftBankedQuarterTurn5TileUp25Seq4, Wide::kLeftBankedQuarterTurn5TileUp25Seq5, Wide::kLeftBankedQuarterTurn5TileUp25Seq6 }}, // LeftBankedQuarterTurn5TileUp25
        {{ Wide::kRightBankedQuarterTurn5TileUp25Seq0, Wide::kRightBankedQuarterTurn5TileUp25Seq1, Wide::kRightBankedQuarterTurn5TileUp25Seq2, Wide::kRightBankedQuarterTurn5TileUp25Seq3, Wide::kRightBankedQuarterTurn5TileUp25Seq4, Wide::kRightBankedQuarterTurn5TileUp25Seq5, Wide::kRightBankedQuarterTurn5TileUp25Seq6 }}, // RightBankedQuarterTurn5TileUp25
        {{ Wide::kLeftBankedQuarterTurn5TileDown25Seq0, Wide::kLeftBankedQuarterTurn5TileDown25Seq1, Wide::kLeftBankedQuarterTurn5TileDown25Seq2, Wide::kLeftBankedQuarterTurn5TileDown25Seq3, Wide::kLeftBankedQuarterTurn5TileDown25Seq4, Wide::kLeftBankedQuarterTurn5TileDown25Seq5, Wide::kLeftBankedQuarterTurn5TileDown25Seq6 }}, // LeftBankedQuarterTurn5TileDown25
        {{ Wide::kRightBankedQuarterTurn5TileDown25Seq0, Wide::kRightBankedQuarterTurn5TileDown25Seq1, Wide::kRightBankedQuarterTurn5TileDown25Seq2, Wide::kRightBankedQuarterTurn5TileDown25Seq3, Wide::kRightBankedQuarterTurn5TileDown25Seq4, Wide::kRightBankedQuarterTurn5TileDown25Seq5, Wide::kRightBankedQuarterTurn5TileDown25Seq6 }}, // RightBankedQuarterTurn5TileDown25
        {{ Wide::kUp25ToLeftBankedUp25Seq0 }}, // Up25ToLeftBankedUp25
        {{ Wide::kUp25ToRightBankedUp25Seq0 }}, // Up25ToRightBankedUp25
        {{ Wide::kLeftBankedUp25ToUp25Seq0 }}, // LeftBankedUp25ToUp25
        {{ Wide::kRightBankedUp25ToUp25Seq0 }}, // RightBankedUp25ToUp25
        {{ Wide::kDown25ToLeftBankedDown25Seq0 }}, // Down25ToLeftBankedDown25
        {{ Wide::kDown25ToRightBankedDown25Seq0 }}, // Down25ToRightBankedDown25
        {{ Wide::kLeftBankedDown25ToDown25Seq0 }}, // LeftBankedDown25ToDown25
        {{ Wide::kRightBankedDown25ToDown25Seq0 }}, // RightBankedDown25ToDown25
        {{ Wide::kLeftBankedFlatToLeftBankedUp25Seq0 }}, // LeftBankedFlatToLeftBankedUp25
        {{ Wide::kRightBankedFlatToRightBankedUp25Seq0 }}, // RightBankedFlatToRightBankedUp25
        {{ Wide::kLeftBankedUp25ToLeftBankedFlatSeq0 }}, // LeftBankedUp25ToLeftBankedFlat
        {{ Wide::kRightBankedUp25ToRightBankedFlatSeq0 }}, // RightBankedUp25ToRightBankedFlat
        {{ Wide::kLeftBankedFlatToLeftBankedDown25Seq0 }}, // LeftBankedFlatToLeftBankedDown25
        {{ Wide::kRightBankedFlatToRightBankedDown25Seq0 }}, // RightBankedFlatToRightBankedDown25
        {{ Wide::kLeftBankedDown25ToLeftBankedFlatSeq0 }}, // LeftBankedDown25ToLeftBankedFlat
        {{ Wide::kRightBankedDown25ToRightBankedFlatSeq0 }}, // RightBankedDown25ToRightBankedFlat
        {{ Wide::kFlatToLeftBankedUp25Seq0 }}, // FlatToLeftBankedUp25
        {{ Wide::kFlatToRightBankedUp25Seq0 }}, // FlatToRightBankedUp25
        {{ Wide::kLeftBankedUp25ToFlatSeq0 }}, // LeftBankedUp25ToFlat
        {{ Wide::kRightBankedUp25ToFlatSeq0 }}, // RightBankedUp25ToFlat
        {{ Wide::kFlatToLeftBankedDown25Seq0 }}, // FlatToLeftBankedDown25
        {{ Wide::kFlatToRightBankedDown25Seq0 }}, // FlatToRightBankedDown25
        {{ Wide::kLeftBankedDown25ToFlatSeq0 }}, // LeftBankedDown25ToFlat
        {{ Wide::kRightBankedDown25ToFlatSeq0 }}, // RightBankedDown25ToFlat
        {{ Wide::kLeftQuarterTurn1TileUp90Seq0, Wide::kLeftQuarterTurn1TileUp90Seq1 }}, // LeftQuarterTurn1TileUp90
        {{ Wide::kRightQuarterTurn1TileUp90Seq0, Wide::kRightQuarterTurn1TileUp90Seq1 }}, // RightQuarterTurn1TileUp90
        {{ Wide::kLeftQuarterTurn1TileDown90Seq0, Wide::kLeftQuarterTurn1TileDown90Seq1 }}, // LeftQuarterTurn1TileDown90
        {{ Wide::kRightQuarterTurn1TileDown90Seq0, Wide::kRightQuarterTurn1TileDown90Seq1 }}, // RightQuarterTurn1TileDown90
        {{ Wide::kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, Wide::kMultiDimUp90ToInvertedFlatQuarterLoopSeq1, Wide::kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 }}, // MultiDimUp90ToInvertedFlatQuarterLoop
        {{ Wide::kMultiDimFlatToDown90QuarterLoopSeq0, Wide::kMultiDimFlatToDown90QuarterLoopSeq1, Wide::kMultiDimFlatToDown90QuarterLoopSeq2, Wide::kMultiDimFlatToDown90QuarterLoopSeq3 }}, // MultiDimFlatToDown90QuarterLoop
        {{ Wide::kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, Wide::kMultiDimInvertedUp90ToFlatQuarterLoopSeq1, Wide::kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 }}, // MultiDimInvertedUp90ToFlatQuarterLoop
        {{ Wide::kRotationControlToggleSeq0 }}, // RotationControlToggle
        {{ Wide::kFlatTrack1x4ASeq0, Wide::kFlatTrack1x4ASeq1, Wide::kFlatTrack1x4ASeq2, Wide::kFlatTrack1x4ASeq3 }}, // FlatTrack1x4A
        {{ Wide::kFlatTrack2x2Seq0, Wide::kFlatTrack2x2Seq1, Wide::kFlatTrack2x2Seq2, Wide::kFlatTrack2x2Seq3 }}, // FlatTrack2x2
        {{ Wide::kFlatTrack4x4Seq0, Wide::kFlatTrack4x4Seq1, Wide::kFlatTrack4x4Seq2, Wide::kFlatTrack4x4Seq3, Wide::kFlatTrack4x4Seq4, Wide::kFlatTrack4x4Seq5, Wide::kFlatTrack4x4Seq6, Wide::kFlatTrack4x4Seq7, Wide::kFlatTrack4x4Seq8, Wide::kFlatTrack4x4Seq9, Wide::kFlatTrack4x4Seq10, Wide::kFlatTrack4x4Seq11, Wide::kFlatTrack4x4Seq12, Wide::kFlatTrack4x4Seq13, Wide::kFlatTrack4x4Seq14, Wide::kFlatTrack4x4Seq15 }}, // FlatTrack4x4
        {{ Wide::kFlatTrack2x4Seq0, Wide::kFlatTrack2x4Seq1, Wide::kFlatTrack2x4Seq2, Wide::kFlatTrack2x4Seq3, Wide::kFlatTrack2x4Seq4, Wide::kFlatTrack2x4Seq5, Wide::kFlatTrack2x4Seq6, Wide::kFlatTrack2x4Seq7 }}, // FlatTrack2x4
        {{ Wide::kFlatTrack1x5Seq0, Wide::kFlatTrack1x5Seq1, Wide::kFlatTrack1x5Seq2, Wide::kFlatTrack1x5Seq3, Wide::kFlatTrack1x5Seq4 }}, // FlatTrack1x5
        {{ Wide::kFlatTrack1x1ASeq0 }}, // FlatTrack1x1A
        {{ Wide::kFlatTrack1x4BSeq0, Wide::kFlatTrack1x4BSeq1, Wide::kFlatTrack1x4BSeq2, Wide::kFlatTrack1x4BSeq3 }}, // FlatTrack1x4B
        {{ Wide::kFlatTrack1x1BSeq0 }}, // FlatTrack1x1B
        {{ Wide::kFlatTrack1x4CSeq0, Wide::kFlatTrack1x4CSeq1, Wide::kFlatTrack1x4CSeq2, Wide::kFlatTrack1x4CSeq3 }}, // FlatTrack1x4C
        {{ Wide::kFlatTrack3x3Seq0, Wide::kFlatTrack3x3Seq1, Wide::kFlatTrack3x3Seq2, Wide::kFlatTrack3x3Seq3, Wide::kFlatTrack3x3Seq4, Wide::kFlatTrack3x3Seq5, Wide::kFlatTrack3x3Seq6, Wide::kFlatTrack3x3Seq7, Wide::kFlatTrack3x3Seq8 }}, // FlatTrack3x3
        {{ Wide::kLeftLargeCorkscrewUpSeq0, Wide::kLeftLargeCorkscrewUpSeq1, Wide::kLeftLargeCorkscrewUpSeq2, Wide::kLeftLargeCorkscrewUpSeq3, Wide::kLeftLargeCorkscrewUpSeq4, Wide::kLeftLargeCorkscrewUpSeq5 }}, // LeftLargeCorkscrewUp
        {{ Wide::kRightLargeCorkscrewUpSeq0, Wide::kRightLargeCorkscrewUpSeq1, Wide::kRightLargeCorkscrewUpSeq2, Wide::kRightLargeCorkscrewUpSeq3, Wide::kRightLargeCorkscrewUpSeq4, Wide::kRightLargeCorkscrewUpSeq5 }}, // RightLargeCorkscrewUp
        {{ Wide::kLeftLargeCorkscrewDownSeq0, Wide::kLeftLargeCorkscrewDownSeq1, Wide::kLeftLargeCorkscrewDownSeq2, Wide::kLeftLargeCorkscrewDownSeq3, Wide::kLeftLargeCorkscrewDownSeq4, Wide::kLeftLargeCorkscrewDownSeq5 }}, // LeftLargeCorkscrewDown
        {{ Wide::kRightLargeCorkscrewDownSeq0, Wide::kRightLargeCorkscrewDownSeq1, Wide::kRightLargeCorkscrewDownSeq2, Wide::kRightLargeCorkscrewDownSeq3, Wide::kRightLargeCorkscrewDownSeq4, Wide::kRightLargeCorkscrewDownSeq5 }}, // RightLargeCorkscrewDown
        {{ Wide::kLeftMediumHalfLoopUpSeq0, Wide::kLeftMediumHalfLoopUpSeq1, Wide::kLeftMediumHalfLoopUpSeq2, Wide::kLeftMediumHalfLoopUpSeq3, Wide::kLeftMediumHalfLoopUpSeq4 }}, // LeftMediumHalfLoopUp
        {{ Wide::kRightMediumHalfLoopUpSeq0, Wide::kRightMediumHalfLoopUpSeq1, Wide::kRightMediumHalfLoopUpSeq2, Wide::kRightMediumHalfLoopUpSeq3, Wide::kRightMediumHalfLoopUpSeq4 }}, // RightMediumHalfLoopUp
        {{ Wide::kLeftMediumHalfLoopDownSeq0, Wide::kLeftMediumHalfLoopDownSeq1, Wide::kLeftMediumHalfLoopDownSeq2, Wide::kLeftMediumHalfLoopDownSeq3, Wide::kLeftMediumHalfLoopDownSeq4 }}, // LeftMediumHalfLoopDown
        {{ Wide::kRightMediumHalfLoopDownSeq0, Wide::kRightMediumHalfLoopDownSeq1, Wide::kRightMediumHalfLoopDownSeq2, Wide::kRightMediumHalfLoopDownSeq3, Wide::kRightMediumHalfLoopDownSeq4 }}, // RightMediumHalfLoopDown
        {{ Wide::kLeftZeroGRollUpSeq0, Wide::kLeftZeroGRollUpSeq1, Wide::kLeftZeroGRollUpSeq2 }}, // LeftZeroGRollUp
        {{ Wide::kRightZeroGRollUpSeq0, Wide::kRightZeroGRollUpSeq1, Wide::kRightZeroGRollUpSeq2 }}, // RightZeroGRollUp
        {{ Wide::kLeftZeroGRollDownSeq0, Wide::kLeftZeroGRollDownSeq1, Wide::kLeftZeroGRollDownSeq2 }}, // LeftZeroGRollDown
        {{ Wide::kRightZeroGRollDownSeq0, Wide::kRightZeroGRollDownSeq1, Wide::kRightZeroGRollDownSeq2 }}, // RightZeroGRollDown
        {{ Wide::kLeftLargeZeroGRollUpSeq0, Wide::kLeftLargeZeroGRollUpSeq1, Wide::kLeftLargeZeroGRollUpSeq2, Wide::kLeftLargeZeroGRollUpSeq3 }}, // LeftLargeZeroGRollUp
        {{ Wide::kRightLargeZeroGRollUpSeq0, Wide::kRightLargeZeroGRollUpSeq1, Wide::kRightLargeZeroGRollUpSeq2, Wide::kRightLargeZeroGRollUpSeq3 }}, // RightLargeZeroGRollUp
        {{ Wide::kLeftLargeZeroGRollDownSeq0, Wide::kLeftLargeZeroGRollDownSeq1, Wide::kLeftLargeZeroGRollDownSeq2, Wide::kLeftLargeZeroGRollDownSeq3 }}, // LeftLargeZeroGRollDown
        {{ Wide::kRightLargeZeroGRollDownSeq0, Wide::kRightLargeZeroGRollDownSeq1, Wide::kRightLargeZeroGRollDownSeq2, Wide::kRightLargeZeroGRollDownSeq3 }}, // RightLargeZeroGRollDown
        {{ Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq0, Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq1, Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq2, Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq3, Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq4, Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq5, Wide::kLeftFlyerLargeHalfLoopUninvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedUp
        {{ Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq0, Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq1, Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq2, Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq3, Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq4, Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq5, Wide::kRightFlyerLargeHalfLoopUninvertedUpSeq6 }}, // RightFlyerLargeHalfLoopUninvertedUp
        {{ Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq0, Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq1, Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq2, Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq3, Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq4, Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq5, Wide::kLeftFlyerLargeHalfLoopInvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopInvertedDown
        {{ Wide::kRightFlyerLargeHalfLoopInvertedDownSeq0, Wide::kRightFlyerLargeHalfLoopInvertedDownSeq1, Wide::kRightFlyerLargeHalfLoopInvertedDownSeq2, Wide::kRightFlyerLargeHalfLoopInvertedDownSeq3, Wide::kRightFlyerLargeHalfLoopInvertedDownSeq4, Wide::kRightFlyerLargeHalfLoopInvertedDownSeq5, Wide::kRightFlyerLargeHalfLoopInvertedDownSeq6 }}, // RightFlyerLargeHalfLoopInvertedDown
        {{ Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq0, Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq1, Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq2, Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq3, Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq4, Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq5, Wide::kLeftFlyerLargeHalfLoopInvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopInvertedUp
        {{ Wide::kRightFlyerLargeHalfLoopInvertedUpSeq0, Wide::kRightFlyerLargeHalfLoopInvertedUpSeq1, Wide::kRightFlyerLargeHalfLoopInvertedUpSeq2, Wide::kRightFlyerLargeHalfLoopInvertedUpSeq3, Wide::kRightFlyerLargeHalfLoopInvertedUpSeq4, Wide::kRightFlyerLargeHalfLoopInvertedUpSeq5, Wide::kRightFlyerLargeHalfLoopInvertedUpSeq6 }}, // RightFlyerLargeHalfLoopInvertedUp
        {{ Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq0, Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq1, Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq2, Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq3, Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq4, Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq5, Wide::kLeftFlyerLargeHalfLoopUninvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedDown
        {{ Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq0, Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq1, Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq2, Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq3, Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq4, Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq5, Wide::kRightFlyerLargeHalfLoopUninvertedDownSeq6 }}, // RightFlyerLargeHalfLoopUninvertedDown
        {{ Wide::kFlyerHalfLoopInvertedUpSeq0, Wide::kFlyerHalfLoopInvertedUpSeq1, Wide::kFlyerHalfLoopInvertedUpSeq2, Wide::kFlyerHalfLoopInvertedUpSeq3 }}, // FlyerHalfLoopInvertedUp
        {{ Wide::kFlyerHalfLoopUninvertedDownSeq0, Wide::kFlyerHalfLoopUninvertedDownSeq1, Wide::kFlyerHalfLoopUninvertedDownSeq2, Wide::kFlyerHalfLoopUninvertedDownSeq3 }}, // FlyerHalfLoopUninvertedDown
        {{ Wide::kLeftEighthToDiagUp25Seq0, Wide::kLeftEighthToDiagUp25Seq1, Wide::kLeftEighthToDiagUp25Seq2, Wide::kLeftEighthToDiagUp25Seq3, Wide::kLeftEighthToDiagUp25Seq4 }}, // LeftEighthToDiagUp25
        {{ Wide::kRightEighthToDiagUp25Seq0, Wide::kRightEighthToDiagUp25Seq1, Wide::kRightEighthToDiagUp25Seq2, Wide::kRightEighthToDiagUp25Seq3, Wide::kRightEighthToDiagUp25Seq4 }}, // RightEighthToDiagUp25
        {{ Wide::kLeftEighthToDiagDown25Seq0, Wide::kLeftEighthToDiagDown25Seq1, Wide::kLeftEighthToDiagDown25Seq2, Wide::kLeftEighthToDiagDown25Seq3, Wide::kLeftEighthToDiagDown25Seq4 }}, // LeftEighthToDiagDown25
        {{ Wide::kRightEighthToDiagDown25Seq0, Wide::kRightEighthToDiagDown25Seq1, Wide::kRightEighthToDiagDown25Seq2, Wide::kRightEighthToDiagDown25Seq3, Wide::kRightEighthToDiagDown25Seq4 }}, // RightEighthToDiagDown25
        {{ Wide::kLeftEighthToOrthogonalUp25Seq0, Wide::kLeftEighthToOrthogonalUp25Seq1, Wide::kLeftEighthToOrthogonalUp25Seq2, Wide::kLeftEighthToOrthogonalUp25Seq3, Wide::kLeftEighthToOrthogonalUp25Seq4 }}, // LeftEighthToOrthogonalUp25
        {{ Wide::kRightEighthToOrthogonalUp25Seq0, Wide::kRightEighthToOrthogonalUp25Seq1, Wide::kRightEighthToOrthogonalUp25Seq2, Wide::kRightEighthToOrthogonalUp25Seq3, Wide::kRightEighthToOrthogonalUp25Seq4 }}, // RightEighthToOrthogonalUp25
        {{ Wide::kLeftEighthToOrthogonalDown25Seq0, Wide::kLeftEighthToOrthogonalDown25Seq1, Wide::kLeftEighthToOrthogonalDown25Seq2, Wide::kLeftEighthToOrthogonalDown25Seq3, Wide::kLeftEighthToOrthogonalDown25Seq4 }}, // LeftEighthToOrthogonalDown25
        {{ Wide::kRightEighthToOrthogonalDown25Seq0, Wide::kRightEighthToOrthogonalDown25Seq1, Wide::kRightEighthToOrthogonalDown25Seq2, Wide::kRightEighthToOrthogonalDown25Seq3, Wide::kRightEighthToOrthogonalDown25Seq4 }}, // RightEighthToOrthogonalDown25
        {{ Wide::kDiagUp25ToLeftBankedUp25Seq0, Wide::kDiagUp25ToLeftBankedUp25Seq1, Wide::kDiagUp25ToLeftBankedUp25Seq2, Wide::kDiagUp25ToLeftBankedUp25Seq3 }}, // DiagUp25ToLeftBankedUp25
        {{ Wide::kDiagUp25ToRightBankedUp25Seq0, Wide::kDiagUp25ToRightBankedUp25Seq1, Wide::kDiagUp25ToRightBankedUp25Seq2, Wide::kDiagUp25ToRightBankedUp25Seq3 }}, // DiagUp25ToRightBankedUp25
        {{ Wide::kDiagLeftBankedUp25ToUp25Seq0, Wide::kDiagLeftBankedUp25ToUp25Seq1, Wide::kDiagLeftBankedUp25ToUp25Seq2, Wide::kDiagLeftBankedUp25ToUp25Seq3 }}, // DiagLeftBankedUp25ToUp25
        {{ Wide::kDiagRightBankedUp25ToUp25Seq0, Wide::kDiagRightBankedUp25ToUp25Seq1, Wide::kDiagRightBankedUp25ToUp25Seq2, Wide::kDiagRightBankedUp25ToUp25Seq3 }}, // DiagRightBankedUp25ToUp25
        {{ Wide::kDiagDown25ToLeftBankedDown25Seq0, Wide::kDiagDown25ToLeftBankedDown25Seq1, Wide::kDiagDown25ToLeftBankedDown25Seq2, Wide::kDiagDown25ToLeftBankedDown25Seq3 }}, // DiagDown25ToLeftBankedDown25
        {{ Wide::kDiagDown25ToRightBankedDown25Seq0, Wide::kDiagDown25ToRightBankedDown25Seq1, Wide::kDiagDown25ToRightBankedDown25Seq2, Wide::kDiagDown25ToRightBankedDown25Seq3 }}, // DiagDown25ToRightBankedDown25
        {{ Wide::kDiagLeftBankedDown25ToDown25Seq0, Wide::kDiagLeftBankedDown25ToDown25Seq1, Wide::kDiagLeftBankedDown25ToDown25Seq2, Wide::kDiagLeftBankedDown25ToDown25Seq3 }}, // DiagLeftBankedDown25ToDown25
        {{ Wide::kDiagRightBankedDown25ToDown25Seq0, Wide::kDiagRightBankedDown25ToDown25Seq1, Wide::kDiagRightBankedDown25ToDown25Seq2, Wide::kDiagRightBankedDown25ToDown25Seq3 }}, // DiagRightBankedDown25ToDown25
        {{ Wide::kDiagLeftBankedFlatToLeftBankedUp25Seq0, Wide::kDiagLeftBankedFlatToLeftBankedUp25Seq1, Wide::kDiagLeftBankedFlatToLeftBankedUp25Seq2, Wide::kDiagLeftBankedFlatToLeftBankedUp25Seq3 }}, // DiagLeftBankedFlatToLeftBankedUp25
        {{ Wide::kDiagRightBankedFlatToRightBankedUp25Seq0, Wide::kDiagRightBankedFlatToRightBankedUp25Seq1, Wide::kDiagRightBankedFlatToRightBankedUp25Seq2, Wide::kDiagRightBankedFlatToRightBankedUp25Seq3 }}, // DiagRightBankedFlatToRightBankedUp25
        {{ Wide::kDiagLeftBankedUp25ToLeftBankedFlatSeq0, Wide::kDiagLeftBankedUp25ToLeftBankedFlatSeq1, Wide::kDiagLeftBankedUp25ToLeftBankedFlatSeq2, Wide::kDiagLeftBankedUp25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedUp25ToLeftBankedFlat
        {{ Wide::kDiagRightBankedUp25ToRightBankedFlatSeq0, Wide::kDiagRightBankedUp25ToRightBankedFlatSeq1, Wide::kDiagRightBankedUp25ToRightBankedFlatSeq2, Wide::kDiagRightBankedUp25ToRightBankedFlatSeq3 }}, // DiagRightBankedUp25ToRightBankedFlat
        {{ Wide::kDiagLeftBankedFlatToLeftBankedDown25Seq0, Wide::kDiagLeftBankedFlatToLeftBankedDown25Seq1, Wide::kDiagLeftBankedFlatToLeftBankedDown25Seq2, Wide::kDiagLeftBankedFlatToLeftBankedDown25Seq3 }}, // DiagLeftBankedFlatToLeftBankedDown25
        {{ Wide::kDiagRightBankedFlatToRightBankedDown25Seq0, Wide::kDiagRightBankedFlatToRightBankedDown25Seq1, Wide::kDiagRightBankedFlatToRightBankedDown25Seq2, Wide::kDiagRightBankedFlatToRightBankedDown25Seq3 }}, // DiagRightBankedFlatToRightBankedDown25
        {{ Wide::kDiagLeftBankedDown25ToLeftBankedFlatSeq0, Wide::kDiagLeftBankedDown25ToLeftBankedFlatSeq1, Wide::kDiagLeftBankedDown25ToLeftBankedFlatSeq2, Wide::kDiagLeftBankedDown25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedDown25ToLeftBankedFlat
        {{ Wide::kDiagRightBankedDown25ToRightBankedFlatSeq0, Wide::kDiagRightBankedDown25ToRightBankedFlatSeq1, Wide::kDiagRightBankedDown25ToRightBankedFlatSeq2, Wide::kDiagRightBankedDown25ToRightBankedFlatSeq3 }}, // DiagRightBankedDown25ToRightBankedFlat
        {{ Wide::kDiagFlatToLeftBankedUp25Seq0, Wide::kDiagFlatToLeftBankedUp25Seq1, Wide::kDiagFlatToLeftBankedUp25Seq2, Wide::kDiagFlatToLeftBankedUp25Seq3 }}, // DiagFlatToLeftBankedUp25
        {{ Wide::kDiagFlatToRightBankedUp25Seq0, Wide::kDiagFlatToRightBankedUp25Seq1, Wide::kDiagFlatToRightBankedUp25Seq2, Wide::kDiagFlatToRightBankedUp25Seq3 }}, // DiagFlatToRightBankedUp25
        {{ Wide::kDiagLeftBankedUp25ToFlatSeq0, Wide::kDiagLeftBankedUp25ToFlatSeq1, Wide::kDiagLeftBankedUp25ToFlatSeq2, Wide::kDiagLeftBankedUp25ToFlatSeq3 }}, // DiagLeftBankedUp25ToFlat
        {{ Wide::kDiagRightBankedUp25ToFlatSeq0, Wide::kDiagRightBankedUp25ToFlatSeq1, Wide::kDiagRightBankedUp25ToFlatSeq2, Wide::kDiagRightBankedUp25ToFlatSeq3 }}, // DiagRightBankedUp25ToFlat
        {{ Wide::kDiagFlatToLeftBankedDown25Seq0, Wide::kDiagFlatToLeftBankedDown25Seq1, Wide::kDiagFlatToLeftBankedDown25Seq2, Wide::kDiagFlatToLeftBankedDown25Seq3 }}, // DiagFlatToLeftBankedDown25
        {{ Wide::kDiagFlatToRightBankedDown25Seq0, Wide::kDiagFlatToRightBankedDown25Seq1, Wide::kDiagFlatToRightBankedDown25Seq2, Wide::kDiagFlatToRightBankedDown25Seq3 }}, // DiagFlatToRightBankedDown25
        {{ Wide::kDiagLeftBankedDown25ToFlatSeq0, Wide::kDiagLeftBankedDown25ToFlatSeq1, Wide::kDiagLeftBankedDown25ToFlatSeq2, Wide::kDiagLeftBankedDown25ToFlatSeq3 }}, // DiagLeftBankedDown25ToFlat
        {{ Wide::kDiagRightBankedDown25ToFlatSeq0, Wide::kDiagRightBankedDown25ToFlatSeq1, Wide::kDiagRightBankedDown25ToFlatSeq2, Wide::kDiagRightBankedDown25ToFlatSeq3 }}, // DiagRightBankedDown25ToFlat
        {{ Wide::kDiagUp25LeftBankedSeq0, Wide::kDiagUp25LeftBankedSeq1, Wide::kDiagUp25LeftBankedSeq2, Wide::kDiagUp25LeftBankedSeq3 }}, // DiagUp25LeftBanked
        {{ Wide::kDiagUp25RightBankedSeq0, Wide::kDiagUp25RightBankedSeq1, Wide::kDiagUp25RightBankedSeq2, Wide::kDiagUp25RightBankedSeq3 }}, // DiagUp25RightBanked
        {{ Wide::kDiagDown25LeftBankedSeq0, Wide::kDiagDown25LeftBankedSeq1, Wide::kDiagDown25LeftBankedSeq2, Wide::kDiagDown25LeftBankedSeq3 }}, // DiagDown25LeftBanked
        {{ Wide::kDiagDown25RightBankedSeq0, Wide::kDiagDown25RightBankedSeq1, Wide::kDiagDown25RightBankedSeq2, Wide::kDiagDown25RightBankedSeq3 }}, // DiagDown25RightBanked
        {{ Wide::kLeftEighthBankToDiagUp25Seq0, Wide::kLeftEighthBankToDiagUp25Seq1, Wide::kLeftEighthBankToDiagUp25Seq2, Wide::kLeftEighthBankToDiagUp25Seq3, Wide::kLeftEighthBankToDiagUp25Seq4 }}, // LeftEighthBankToDiagUp25
        {{ Wide::kRightEighthBankToDiagUp25Seq0, Wide::kRightEighthBankToDiagUp25Seq1, Wide::kRightEighthBankToDiagUp25Seq2, Wide::kRightEighthBankToDiagUp25Seq3, Wide::kRightEighthBankToDiagUp25Seq4 }}, // RightEighthBankToDiagUp25
        {{ Wide::kLeftEighthBankToDiagDown25Seq0, Wide::kLeftEighthBankToDiagDown25Seq1, Wide::kLeftEighthBankToDiagDown25Seq2, Wide::kLeftEighthBankToDiagDown25Seq3, Wide::kLeftEighthBankToDiagDown25Seq4 }}, // LeftEighthBankToDiagDown25
        {{ Wide::kRightEighthBankToDiagDown25Seq0, Wide::kRightEighthBankToDiagDown25Seq1, Wide::kRightEighthBankToDiagDown25Seq2, Wide::kRightEighthBankToDiagDown25Seq3, Wide::kRightEighthBankToDiagDown25Seq4 }}, // RightEighthBankToDiagDown25
        {{ Wide::kLeftEighthBankToOrthogonalUp25Seq0, Wide::kLeftEighthBankToOrthogonalUp25Seq1, Wide::kLeftEighthBankToOrthogonalUp25Seq2, Wide::kLeftEighthBankToOrthogonalUp25Seq3, Wide::kLeftEighthBankToOrthogonalUp25Seq4 }}, // LeftEighthBankToOrthogonalUp25
        {{ Wide::kRightEighthBankToOrthogonalUp25Seq0, Wide::kRightEighthBankToOrthogonalUp25Seq1, Wide::kRightEighthBankToOrthogonalUp25Seq2, Wide::kRightEighthBankToOrthogonalUp25Seq3, Wide::kRightEighthBankToOrthogonalUp25Seq4 }}, // RightEighthBankToOrthogonalUp25
        {{ Wide::kLeftEighthBankToOrthogonalDown25Seq0, Wide::kLeftEighthBankToOrthogonalDown25Seq1, Wide::kLeftEighthBankToOrthogonalDown25Seq2, Wide::kLeftEighthBankToOrthogonalDown25Seq3, Wide::kLeftEighthBankToOrthogonalDown25Seq4 }}, // LeftEighthBankToOrthogonalDown25
        {{ Wide::kRightEighthBankToOrthogonalDown25Seq0, Wide::kRightEighthBankToOrthogonalDown25Seq1, Wide::kRightEighthBankToOrthogonalDown25Seq2, Wide::kRightEighthBankToOrthogonalDown25Seq3, Wide::kRightEighthBankToOrthogonalDown25Seq4 }}, // RightEighthBankToOrthogonalDown25
        {{ Wide::kDiagBrakesSeq0, Wide::kDiagBrakesSeq1, Wide::kDiagBrakesSeq2, Wide::kDiagBrakesSeq3 }}, // DiagBrakes
        {{ Wide::kDiagBlockBrakesSeq0, Wide::kDiagBlockBrakesSeq1, Wide::kDiagBlockBrakesSeq2, Wide::kDiagBlockBrakesSeq3 }}, // DiagBlockBrakes
        {{ Wide::kDown25BrakesSeq0 }}, // Down25Brakes
        {{ Wide::kDiagBoosterSeq0, Wide::kDiagBoosterSeq1, Wide::kDiagBoosterSeq2, Wide::kDiagBoosterSeq3 }}, // DiagBooster
        {{ Wide::kDiagFlatToUp60LongBaseSeq0, Wide::kDiagFlatToUp60LongBaseSeq1, Wide::kDiagFlatToUp60LongBaseSeq2, Wide::kDiagFlatToUp60LongBaseSeq3, Wide::kDiagFlatToUp60LongBaseSeq4, Wide::kDiagFlatToUp60LongBaseSeq5, Wide::kDiagFlatToUp60LongBaseSeq6, Wide::kDiagFlatToUp60LongBaseSeq7, Wide::kDiagFlatToUp60LongBaseSeq8, Wide::kDiagFlatToUp60LongBaseSeq9 }}, // DiagFlatToUp60LongBase
        {{ Wide::kDiagUp60ToFlatLongBaseSeq0, Wide::kDiagUp60ToFlatLongBaseSeq1, Wide::kDiagUp60ToFlatLongBaseSeq2, Wide::kDiagUp60ToFlatLongBaseSeq3, Wide::kDiagUp60ToFlatLongBaseSeq4, Wide::kDiagUp60ToFlatLongBaseSeq5, Wide::kDiagUp60ToFlatLongBaseSeq6, Wide::kDiagUp60ToFlatLongBaseSeq7, Wide::kDiagUp60ToFlatLongBaseSeq8, Wide::kDiagUp60ToFlatLongBaseSeq9 }}, // DiagUp60ToFlatLongBase
        {{ Wide::kDiagFlatToDown60LongBaseSeq0, Wide::kDiagFlatToDown60LongBaseSeq1, Wide::kDiagFlatToDown60LongBaseSeq2, Wide::kDiagFlatToDown60LongBaseSeq3, Wide::kDiagFlatToDown60LongBaseSeq4, Wide::kDiagFlatToDown60LongBaseSeq5, Wide::kDiagFlatToDown60LongBaseSeq6, Wide::kDiagFlatToDown60LongBaseSeq7, Wide::kDiagFlatToDown60LongBaseSeq8, Wide::kDiagFlatToDown60LongBaseSeq9 }}, // DiagFlatToDown60LongBase
        {{ Wide::kDiagDown60ToFlatLongBaseSeq0, Wide::kDiagDown60ToFlatLongBaseSeq1, Wide::kDiagDown60ToFlatLongBaseSeq2, Wide::kDiagDown60ToFlatLongBaseSeq3, Wide::kDiagDown60ToFlatLongBaseSeq4, Wide::kDiagDown60ToFlatLongBaseSeq5, Wide::kDiagDown60ToFlatLongBaseSeq6, Wide::kDiagDown60ToFlatLongBaseSeq7, Wide::kDiagDown60ToFlatLongBaseSeq8, Wide::kDiagDown60ToFlatLongBaseSeq9 }}, // DiagDown60ToFlatLongBase
        {{ Wide::kLeftEighthDiveLoopUpToOrthogonalSeq0, Wide::kLeftEighthDiveLoopUpToOrthogonalSeq1, Wide::kLeftEighthDiveLoopUpToOrthogonalSeq2, Wide::kLeftEighthDiveLoopUpToOrthogonalSeq3, Wide::kLeftEighthDiveLoopUpToOrthogonalSeq4, Wide::kLeftEighthDiveLoopUpToOrthogonalSeq5 }}, // LeftEighthDiveLoopUpToOrthogonal
        {{ Wide::kRightEighthDiveLoopUpToOrthogonalSeq0, Wide::kRightEighthDiveLoopUpToOrthogonalSeq1, Wide::kRightEighthDiveLoopUpToOrthogonalSeq2, Wide::kRightEighthDiveLoopUpToOrthogonalSeq3, Wide::kRightEighthDiveLoopUpToOrthogonalSeq4, Wide::kRightEighthDiveLoopUpToOrthogonalSeq5 }}, // RightEighthDiveLoopUpToOrthogonal
        {{ Wide::kLeftEighthDiveLoopDownToDiagSeq0, Wide::kLeftEighthDiveLoopDownToDiagSeq1, Wide::kLeftEighthDiveLoopDownToDiagSeq2, Wide::kLeftEighthDiveLoopDownToDiagSeq3, Wide::kLeftEighthDiveLoopDownToDiagSeq4, Wide::kLeftEighthDiveLoopDownToDiagSeq5 }}, // LeftEighthDiveLoopDownToDiag
        {{ Wide::kRightEighthDiveLoopDownToDiagSeq0, Wide::kRightEighthDiveLoopDownToDiagSeq1, Wide::kRightEighthDiveLoopDownToDiagSeq2, Wide::kRightEighthDiveLoopDownToDiagSeq3, Wide::kRightEighthDiveLoopDownToDiagSeq4, Wide::kRightEighthDiveLoopDownToDiagSeq5 }}, // RightEighthDiveLoopDownToDiag
        {{ Wide::kDiagDown25BrakesSeq0, Wide::kDiagDown25BrakesSeq1, Wide::kDiagDown25BrakesSeq2, Wide::kDiagDown25BrakesSeq3 }}, // DiagDown25Brakes
    };

    static_assert(std::size(kBlockedSegmentsWide) == EnumValue(TrackElemType::Count));

    // kBlockedSegmentsSuspendedSwingingTrain is the inverted group with the suspended swinging train additions.
    static constexpr const std::array<uint16_t, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> kBlockedSegmentsSuspendedSwingingTrain[] = {
        {{ Inverted::kFlatSeq0 }}, // Flat
        {{ Inverted::kEndStationSeq0 }}, // EndStation
        {{ Inverted::kBeginStationSeq0 }}, // BeginStation
        {{ Inverted::kMiddleStationSeq0 }}, // MiddleStation
        {{ Inverted::kUp25Seq0 }}, // Up25
        {{ Inverted::kUp60Seq0 }}, // Up60
        {{ Inverted::kFlatToUp25Seq0 }}, // FlatToUp25
        {{ Inverted::kUp25ToUp60Seq0 }}, // Up25ToUp60
        {{ Inverted::kUp60ToUp25Seq0 }}, // Up60ToUp25
        {{ Inverted::kUp25ToFlatSeq0 }}, // Up25ToFlat
        {{ Inverted::kDown25Seq0 }}, // Down25
        {{ Inverted::kDown60Seq0 }}, // Down60
        {{ Inverted::kFlatToDown25Seq0 }}, // FlatToDown25
        {{ Inverted::kDown25ToDown60Seq0 }}, // Down25ToDown60
        {{ Inverted::kDown60ToDown25Seq0 }}, // Down60ToDown25
        {{ Inverted::kDown25ToFlatSeq0 }}, // Down25ToFlat
        {{ Inverted::kLeftQuarterTurn5TilesSeq0, Inverted::kLeftQuarterTurn5TilesSeq1, Inverted::kLeftQuarterTurn5TilesSeq2, Inverted::kLeftQuarterTurn5TilesSeq3, Inverted::kLeftQuarterTurn5TilesSeq4, Inverted::kLeftQuarterTurn5TilesSeq5, Inverted::kLeftQuarterTurn5TilesSeq6 }}, // LeftQuarterTurn5Tiles
        {{ Inverted::kRightQuarterTurn5TilesSeq0, Inverted::kRightQuarterTurn5TilesSeq1, Inverted::kRightQuarterTurn5TilesSeq2, Inverted::kRightQuarterTurn5TilesSeq3, Inverted::kRightQuarterTurn5TilesSeq4, Inverted::kRightQuarterTurn5TilesSeq5, Inverted::kRightQuarterTurn5TilesSeq6 }}, // RightQuarterTurn5Tiles
        {{ Inverted::kFlatToLeftBankSeq0 }}, // FlatToLeftBank
        {{ Inverted::kFlatToRightBankSeq0 }}, // FlatToRightBank
        {{ Inverted::kLeftBankToFlatSeq0 }}, // LeftBankToFlat
        {{ Inverted::kRightBankToFlatSeq0 }}, // RightBankToFlat
        {{ Inverted::kBankedLeftQuarterTurn5TilesSeq0, Inverted::kBankedLeftQuarterTurn5TilesSeq1, Inverted::kBankedLeftQuarterTurn5TilesSeq2, Inverted::kBankedLeftQuarterTurn5TilesSeq3, Inverted::kBankedLeftQuarterTurn5TilesSeq4, Inverted::kBankedLeftQuarterTurn5TilesSeq5, Inverted::kBankedLeftQuarterTurn5TilesSeq6 }}, // BankedLeftQuarterTurn5Tiles
        {{ Inverted::kBankedRightQuarterTurn5TilesSeq0, Inverted::kBankedRightQuarterTurn5TilesSeq1, Inverted::kBankedRightQuarterTurn5TilesSeq2, Inverted::kBankedRightQuarterTurn5TilesSeq3, Inverted::kBankedRightQuarterTurn5TilesSeq4, Inverted::kBankedRightQuarterTurn5TilesSeq5, Inverted::kBankedRightQuarterTurn5TilesSeq6 }}, // BankedRightQuarterTurn5Tiles
        {{ Inverted::kLeftBankToUp25Seq0 }}, // LeftBankToUp25
        {{ Inverted::kRightBankToUp25Seq0 }}, // RightBankToUp25
        {{ Inverted::kUp25ToLeftBankSeq0 }}, // Up25ToLeftBank
        {{ Inverted::kUp25ToRightBankSeq0 }}, // Up25ToRightBank
        {{ Inverted::kLeftBankToDown25Seq0 }}, // LeftBankToDown25
        {{ Inverted::kRightBankToDown25Seq0 }}, // RightBankToDown25
        {{ Inverted::kDown25ToLeftBankSeq0 }}, // Down25ToLeftBank
        {{ Inverted::kDown25ToRightBankSeq0 }}, // Down25ToRightBank
        {{ Inverted::kLeftBankSeq0 }}, // LeftBank
        {{ Inverted::kRightBankSeq0 }}, // RightBank
        {{ Inverted::kLeftQuarterTurn5TilesUp25Seq0, Inverted::kLeftQuarterTurn5TilesUp25Seq1, Inverted::kLeftQuarterTurn5TilesUp25Seq2, Inverted::kLeftQuarterTurn5TilesUp25Seq3, Inverted::kLeftQuarterTurn5TilesUp25Seq4, Inverted::kLeftQuarterTurn5TilesUp25Seq5, Inverted::kLeftQuarterTurn5TilesUp25Seq6 }}, // LeftQuarterTurn5TilesUp25
        {{ Inverted::kRightQuarterTurn5TilesUp25Seq0, Inverted::kRightQuarterTurn5TilesUp25Seq1, Inverted::kRightQuarterTurn5TilesUp25Seq2, Inverted::kRightQuarterTurn5TilesUp25Seq3, Inverted::kRightQuarterTurn5TilesUp25Seq4, Inverted::kRightQuarterTurn5TilesUp25Seq5, Inverted::kRightQuarterTurn5TilesUp25Seq6 }}, // RightQuarterTurn5TilesUp25
        {{ Inverted::kLeftQuarterTurn5TilesDown25Seq0, Inverted::kLeftQuarterTurn5TilesDown25Seq1, Inverted::kLeftQuarterTurn5TilesDown25Seq2, Inverted::kLeftQuarterTurn5TilesDown25Seq3, Inverted::kLeftQuarterTurn5TilesDown25Seq4, Inverted::kLeftQuarterTurn5TilesDown25Seq5, Inverted::kLeftQuarterTurn5TilesDown25Seq6 }}, // LeftQuarterTurn5TilesDown25
        {{ Inverted::kRightQuarterTurn5TilesDown25Seq0, Inverted::kRightQuarterTurn5TilesDown25Seq1, Inverted::kRightQuarterTurn5TilesDown25Seq2, Inverted::kRightQuarterTurn5TilesDown25Seq3, Inverted::kRightQuarterTurn5TilesDown25Seq4, Inverted::kRightQuarterTurn5TilesDown25Seq5, Inverted::kRightQuarterTurn5TilesDown25Seq6 }}, // RightQuarterTurn5TilesDown25
        {{ Inverted::kSBendLeftSeq0, Inverted::kSBendLeftSeq1, Inverted::kSBendLeftSeq2, Inverted::kSBendLeftSeq3 }}, // SBendLeft
        {{ Inverted::kSBendRightSeq0, Inverted::kSBendRightSeq1, Inverted::kSBendRightSeq2, Inverted::kSBendRightSeq3 }}, // SBendRight
        {{ Inverted::kLeftVerticalLoopSeq0, Inverted::kLeftVerticalLoopSeq1, Inverted::kLeftVerticalLoopSeq2, Inverted::kLeftVerticalLoopSeq3, Inverted::kLeftVerticalLoopSeq4, Inverted::kLeftVerticalLoopSeq5, Inverted::kLeftVerticalLoopSeq6, Inverted::kLeftVerticalLoopSeq7, Inverted::kLeftVerticalLoopSeq8, Inverted::kLeftVerticalLoopSeq9 }}, // LeftVerticalLoop
        {{ Inverted::kRightVerticalLoopSeq0, Inverted::kRightVerticalLoopSeq1, Inverted::kRightVerticalLoopSeq2, Inverted::kRightVerticalLoopSeq3, Inverted::kRightVerticalLoopSeq4, Inverted::kRightVerticalLoopSeq5, Inverted::kRightVerticalLoopSeq6, Inverted::kRightVerticalLoopSeq7, Inverted::kRightVerticalLoopSeq8, Inverted::kRightVerticalLoopSeq9 }}, // RightVerticalLoop
        {{ Inverted::kLeftQuarterTurn3TilesSeq0, Inverted::kLeftQuarterTurn3TilesSeq1, Inverted::kLeftQuarterTurn3TilesSeq2, Inverted::kLeftQuarterTurn3TilesSeq3 }}, // LeftQuarterTurn3Tiles
        {{ Inverted::kRightQuarterTurn3TilesSeq0, Inverted::kRightQuarterTurn3TilesSeq1, Inverted::kRightQuarterTurn3TilesSeq2, Inverted::kRightQuarterTurn3TilesSeq3 }}, // RightQuarterTurn3Tiles
        {{ Inverted::kLeftBankedQuarterTurn3TilesSeq0, Inverted::kLeftBankedQuarterTurn3TilesSeq1, Inverted::kLeftBankedQuarterTurn3TilesSeq2, Inverted::kLeftBankedQuarterTurn3TilesSeq3 }}, // LeftBankedQuarterTurn3Tiles
        {{ Inverted::kRightBankedQuarterTurn3TilesSeq0, Inverted::kRightBankedQuarterTurn3TilesSeq1, Inverted::kRightBankedQuarterTurn3TilesSeq2, Inverted::kRightBankedQuarterTurn3TilesSeq3 }}, // RightBankedQuarterTurn3Tiles
        {{ Inverted::kLeftQuarterTurn3TilesUp25Seq0, Inverted::kLeftQuarterTurn3TilesUp25Seq1, Inverted::kLeftQuarterTurn3TilesUp25Seq2, Inverted::kLeftQuarterTurn3TilesUp25Seq3 }}, // LeftQuarterTurn3TilesUp25
        {{ Inverted::kRightQuarterTurn3TilesUp25Seq0, Inverted::kRightQuarterTurn3TilesUp25Seq1, Inverted::kRightQuarterTurn3TilesUp25Seq2, Inverted::kRightQuarterTurn3TilesUp25Seq3 }}, // RightQuarterTurn3TilesUp25
        {{ Inverted::kLeftQuarterTurn3TilesDown25Seq0, Inverted::kLeftQuarterTurn3TilesDown25Seq1, Inverted::kLeftQuarterTurn3TilesDown25Seq2, Inverted::kLeftQuarterTurn3TilesDown25Seq3 }}, // LeftQuarterTurn3TilesDown25
        {{ Inverted::kRightQuarterTurn3TilesDown25Seq0, Inverted::kRightQuarterTurn3TilesDown25Seq1, Inverted::kRightQuarterTurn3TilesDown25Seq2, Inverted::kRightQuarterTurn3TilesDown25Seq3 }}, // RightQuarterTurn3TilesDown25
        {{ Inverted::kLeftQuarterTurn1TileSeq0 }}, // LeftQuarterTurn1Tile
        {{ Inverted::kRightQuarterTurn1TileSeq0 }}, // RightQuarterTurn1Tile
        {{ Inverted::kLeftTwistDownToUpSeq0, Inverted::kLeftTwistDownToUpSeq1, Inverted::kLeftTwistDownToUpSeq2 }}, // LeftTwistDownToUp
        {{ Inverted::kRightTwistDownToUpSeq0, Inverted::kRightTwistDownToUpSeq1, Inverted::kRightTwistDownToUpSeq2 }}, // RightTwistDownToUp
        {{ Inverted::kLeftTwistUpToDownSeq0, Inverted::kLeftTwistUpToDownSeq1, Inverted::kLeftTwistUpToDownSeq2 }}, // LeftTwistUpToDown
        {{ Inverted::kRightTwistUpToDownSeq0, Inverted::kRightTwistUpToDownSeq1, Inverted::kRightTwistUpToDownSeq2 }}, // RightTwistUpToDown
        {{ Inverted::kHalfLoopUpSeq0, Inverted::kHalfLoopUpSeq1, Inverted::kHalfLoopUpSeq2, Inverted::kHalfLoopUpSeq3 }}, // HalfLoopUp
        {{ Inverted::kHalfLoopDownSeq0, Inverted::kHalfLoopDownSeq1, Inverted::kHalfLoopDownSeq2, Inverted::kHalfLoopDownSeq3 }}, // HalfLoopDown
        {{ Inverted::kLeftCorkscrewUpSeq0, Inverted::kLeftCorkscrewUpSeq1, Inverted::kLeftCorkscrewUpSeq2 }}, // LeftCorkscrewUp
        {{ Inverted::kRightCorkscrewUpSeq0, Inverted::kRightCorkscrewUpSeq1, Inverted::kRightCorkscrewUpSeq2 }}, // RightCorkscrewUp
        {{ Inverted::kLeftCorkscrewDownSeq0, Inverted::kLeftCorkscrewDownSeq1, Inverted::kLeftCorkscrewDownSeq2 }}, // LeftCorkscrewDown
        {{ Inverted::kRightCorkscrewDownSeq0, Inverted::kRightCorkscrewDownSeq1, Inverted::kRightCorkscrewDownSeq2 }}, // RightCorkscrewDown
        {{ Inverted::kFlatToUp60Seq0 }}, // FlatToUp60
        {{ Inverted::kUp60ToFlatSeq0 }}, // Up60ToFlat
        {{ Inverted::kFlatToDown60Seq0 }}, // FlatToDown60
        {{ Inverted::kDown60ToFlatSeq0 }}, // Down60ToFlat
        {{ Inverted::kTowerBaseSeq0, Inverted::kTowerBaseSeq1, Inverted::kTowerBaseSeq2, Inverted::kTowerBaseSeq3, Inverted::kTowerBaseSeq4, Inverted::kTowerBaseSeq5, Inverted::kTowerBaseSeq6, Inverted::kTowerBaseSeq7, Inverted::kTowerBaseSeq8 }}, // TowerBase
        {{ Inverted::kTowerSectionSeq0, Inverted::kTowerSectionSeq1 }}, // TowerSection
        {{ Inverted::kFlatCoveredSeq0 }}, // FlatCovered
        {{ Inverted::kUp25CoveredSeq0 }}, // Up25Covered
        {{ Inverted::kUp60CoveredSeq0 }}, // Up60Covered
        {{ Inverted::kFlatToUp25CoveredSeq0 }}, // FlatToUp25Covered
        {{ Inverted::kUp25ToUp60CoveredSeq0 }}, // Up25ToUp60Covered
        {{ Inverted::kUp60ToUp25CoveredSeq0 }}, // Up60ToUp25Covered
        {{ Inverted::kUp25ToFlatCoveredSeq0 }}, // Up25ToFlatCovered
        {{ Inverted::kDown25CoveredSeq0 }}, // Down25Covered
        {{ Inverted::kDown60CoveredSeq0 }}, // Down60Covered
        {{ Inverted::kFlatToDown25CoveredSeq0 }}, // FlatToDown25Covered
        {{ Inverted::kDown25ToDown60CoveredSeq0 }}, // Down25ToDown60Covered
        {{ Inverted::kDown60ToDown25CoveredSeq0 }}, // Down60ToDown25Covered
        {{ Inverted::kDown25ToFlatCoveredSeq0 }}, // Down25ToFlatCovered
        {{ Inverted::kLeftQuarterTurn5TilesCoveredSeq0, Inverted::kLeftQuarterTurn5TilesCoveredSeq1, Inverted::kLeftQuarterTurn5TilesCoveredSeq2, Inverted::kLeftQuarterTurn5TilesCoveredSeq3, Inverted::kLeftQuarterTurn5TilesCoveredSeq4, Inverted::kLeftQuarterTurn5TilesCoveredSeq5, Inverted::kLeftQuarterTurn5TilesCoveredSeq6 }}, // LeftQuarterTurn5TilesCovered
        {{ Inverted::kRightQuarterTurn5TilesCoveredSeq0, Inverted::kRightQuarterTurn5TilesCoveredSeq1, Inverted::kRightQuarterTurn5TilesCoveredSeq2, Inverted::kRightQuarterTurn5TilesCoveredSeq3, Inverted::kRightQuarterTurn5TilesCoveredSeq4, Inverted::kRightQuarterTurn5TilesCoveredSeq5, Inverted::kRightQuarterTurn5TilesCoveredSeq6 }}, // RightQuarterTurn5TilesCovered
        {{ Inverted::kSBendLeftCoveredSeq0, Inverted::kSBendLeftCoveredSeq1, Inverted::kSBendLeftCoveredSeq2, Inverted::kSBendLeftCoveredSeq3 }}, // SBendLeftCovered
        {{ Inverted::kSBendRightCoveredSeq0, Inverted::kSBendRightCoveredSeq1, Inverted::kSBendRightCoveredSeq2, Inverted::kSBendRightCoveredSeq3 }}, // SBendRightCovered
        {{ Inverted::kLeftQuarterTurn3TilesCoveredSeq0, Inverted::kLeftQuarterTurn3TilesCoveredSeq1, Inverted::kLeftQuarterTurn3TilesCoveredSeq2, Inverted::kLeftQuarterTurn3TilesCoveredSeq3 }}, // LeftQuarterTurn3TilesCovered
        {{ Inverted::kRightQuarterTurn3TilesCoveredSeq0, Inverted::kRightQuarterTurn3TilesCoveredSeq1, Inverted::kRightQuarterTurn3TilesCoveredSeq2, Inverted::kRightQuarterTurn3TilesCoveredSeq3 }}, // RightQuarterTurn3TilesCovered
        {{ Inverted::kLeftHalfBankedHelixUpSmallSeq0, Inverted::kLeftHalfBankedHelixUpSmallSeq1, Inverted::kLeftHalfBankedHelixUpSmallSeq2, Inverted::kLeftHalfBankedHelixUpSmallSeq3, Inverted::kLeftHalfBankedHelixUpSmallSeq4, Inverted::kLeftHalfBankedHelixUpSmallSeq5, Inverted::kLeftHalfBankedHelixUpSmallSeq6, Inverted::kLeftHalfBankedHelixUpSmallSeq7 }}, // LeftHalfBankedHelixUpSmall
        {{ Inverted::kRightHalfBankedHelixUpSmallSeq0, Inverted::kRightHalfBankedHelixUpSmallSeq1, Inverted::kRightHalfBankedHelixUpSmallSeq2, Inverted::kRightHalfBankedHelixUpSmallSeq3, Inverted::kRightHalfBankedHelixUpSmallSeq4, Inverted::kRightHalfBankedHelixUpSmallSeq5, Inverted::kRightHalfBankedHelixUpSmallSeq6, Inverted::kRightHalfBankedHelixUpSmallSeq7 }}, // RightHalfBankedHelixUpSmall
        {{ Inverted::kLeftHalfBankedHelixDownSmallSeq0, Inverted::kLeftHalfBankedHelixDownSmallSeq1, Inverted::kLeftHalfBankedHelixDownSmallSeq2, Inverted::kLeftHalfBankedHelixDownSmallSeq3, Inverted::kLeftHalfBankedHelixDownSmallSeq4, Inverted::kLeftHalfBankedHelixDownSmallSeq5, Inverted::kLeftHalfBankedHelixDownSmallSeq6, Inverted::kLeftHalfBankedHelixDownSmallSeq7 }}, // LeftHalfBankedHelixDownSmall
        {{ Inverted::kRightHalfBankedHelixDownSmallSeq0, Inverted::kRightHalfBankedHelixDownSmallSeq1, Inverted::kRightHalfBankedHelixDownSmallSeq2, Inverted::kRightHalfBankedHelixDownSmallSeq3, Inverted::kRightHalfBankedHelixDownSmallSeq4, Inverted::kRightHalfBankedHelixDownSmallSeq5, Inverted::kRightHalfBankedHelixDownSmallSeq6, Inverted::kRightHalfBankedHelixDownSmallSeq7 }}, // RightHalfBankedHelixDownSmall
        {{ Inverted::kLeftHalfBankedHelixUpLargeSeq0, Inverted::kLeftHalfBankedHelixUpLargeSeq1, Inverted::kLeftHalfBankedHelixUpLargeSeq2, Inverted::kLeftHalfBankedHelixUpLargeSeq3, Inverted::kLeftHalfBankedHelixUpLargeSeq4, Inverted::kLeftHalfBankedHelixUpLargeSeq5, Inverted::kLeftHalfBankedHelixUpLargeSeq6, Inverted::kLeftHalfBankedHelixUpLargeSeq7, Inverted::kLeftHalfBankedHelixUpLargeSeq8, Inverted::kLeftHalfBankedHelixUpLargeSeq9, Inverted::kLeftHalfBankedHelixUpLargeSeq10, Inverted::kLeftHalfBankedHelixUpLargeSeq11, Inverted::kLeftHalfBankedHelixUpLargeSeq12, Inverted::kLeftHalfBankedHelixUpLargeSeq13 }}, // LeftHalfBankedHelixUpLarge
        {{ Inverted::kRightHalfBankedHelixUpLargeSeq0, Inverted::kRightHalfBankedHelixUpLargeSeq1, Inverted::kRightHalfBankedHelixUpLargeSeq2, Inverted::kRightHalfBankedHelixUpLargeSeq3, Inverted::kRightHalfBankedHelixUpLargeSeq4, Inverted::kRightHalfBankedHelixUpLargeSeq5, Inverted::kRightHalfBankedHelixUpLargeSeq6, Inverted::kRightHalfBankedHelixUpLargeSeq7, Inverted::kRightHalfBankedHelixUpLargeSeq8, Inverted::kRightHalfBankedHelixUpLargeSeq9, Inverted::kRightHalfBankedHelixUpLargeSeq10, Inverted::kRightHalfBankedHelixUpLargeSeq11, Inverted::kRightHalfBankedHelixUpLargeSeq12, Inverted::kRightHalfBankedHelixUpLargeSeq13 }}, // RightHalfBankedHelixUpLarge
        {{ Inverted::kLeftHalfBankedHelixDownLargeSeq0, Inverted::kLeftHalfBankedHelixDownLargeSeq1, Inverted::kLeftHalfBankedHelixDownLargeSeq2, Inverted::kLeftHalfBankedHelixDownLargeSeq3, Inverted::kLeftHalfBankedHelixDownLargeSeq4, Inverted::kLeftHalfBankedHelixDownLargeSeq5, Inverted::kLeftHalfBankedHelixDownLargeSeq6, Inverted::kLeftHalfBankedHelixDownLargeSeq7, Inverted::kLeftHalfBankedHelixDownLargeSeq8, Inverted::kLeftHalfBankedHelixDownLargeSeq9, Inverted::kLeftHalfBankedHelixDownLargeSeq10, Inverted::kLeftHalfBankedHelixDownLargeSeq11, Inverted::kLeftHalfBankedHelixDownLargeSeq12, Inverted::kLeftHalfBankedHelixDownLargeSeq13 }}, // LeftHalfBankedHelixDownLarge
        {{ Inverted::kRightHalfBankedHelixDownLargeSeq0, Inverted::kRightHalfBankedHelixDownLargeSeq1, Inverted::kRightHalfBankedHelixDownLargeSeq2, Inverted::kRightHalfBankedHelixDownLargeSeq3, Inverted::kRightHalfBankedHelixDownLargeSeq4, Inverted::kRightHalfBankedHelixDownLargeSeq5, Inverted::kRightHalfBankedHelixDownLargeSeq6, Inverted::kRightHalfBankedHelixDownLargeSeq7, Inverted::kRightHalfBankedHelixDownLargeSeq8, Inverted::kRightHalfBankedHelixDownLargeSeq9, Inverted::kRightHalfBankedHelixDownLargeSeq10, Inverted::kRightHalfBankedHelixDownLargeSeq11, Inverted::kRightHalfBankedHelixDownLargeSeq12, Inverted::kRightHalfBankedHelixDownLargeSeq13 }}, // RightHalfBankedHelixDownLarge
        {{ Inverted::kLeftQuarterTurn1TileUp60Seq0 }}, // LeftQuarterTurn1TileUp60
        {{ Inverted::kRightQuarterTurn1TileUp60Seq0 }}, // RightQuarterTurn1TileUp60
        {{ Inverted::kLeftQuarterTurn1TileDown60Seq0 }}, // LeftQuarterTurn1TileDown60
        {{ Inverted::kRightQuarterTurn1TileDown60Seq0 }}, // RightQuarterTurn1TileDown60
        {{ Inverted::kBrakesSeq0 }}, // Brakes
        {{ Inverted::kBoosterSeq0 }}, // Booster
        {{ Inverted::kMazeSeq0 }}, // Maze
        {{ Inverted::kLeftQuarterBankedHelixLargeUpSeq0, Inverted::kLeftQuarterBankedHelixLargeUpSeq1, Inverted::kLeftQuarterBankedHelixLargeUpSeq2, Inverted::kLeftQuarterBankedHelixLargeUpSeq3, Inverted::kLeftQuarterBankedHelixLargeUpSeq4, Inverted::kLeftQuarterBankedHelixLargeUpSeq5, Inverted::kLeftQuarterBankedHelixLargeUpSeq6 }}, // LeftQuarterBankedHelixLargeUp
        {{ Inverted::kRightQuarterBankedHelixLargeUpSeq0, Inverted::kRightQuarterBankedHelixLargeUpSeq1, Inverted::kRightQuarterBankedHelixLargeUpSeq2, Inverted::kRightQuarterBankedHelixLargeUpSeq3, Inverted::kRightQuarterBankedHelixLargeUpSeq4, Inverted::kRightQuarterBankedHelixLargeUpSeq5, Inverted::kRightQuarterBankedHelixLargeUpSeq6 }}, // RightQuarterBankedHelixLargeUp
        {{ Inverted::kLeftQuarterBankedHelixLargeDownSeq0, Inverted::kLeftQuarterBankedHelixLargeDownSeq1, Inverted::kLeftQuarterBankedHelixLargeDownSeq2, Inverted::kLeftQuarterBankedHelixLargeDownSeq3, Inverted::kLeftQuarterBankedHelixLargeDownSeq4, Inverted::kLeftQuarterBankedHelixLargeDownSeq5, Inverted::kLeftQuarterBankedHelixLargeDownSeq6 }}, // LeftQuarterBankedHelixLargeDown
        {{ Inverted::kRightQuarterBankedHelixLargeDownSeq0, Inverted::kRightQuarterBankedHelixLargeDownSeq1, Inverted::kRightQuarterBankedHelixLargeDownSeq2, Inverted::kRightQuarterBankedHelixLargeDownSeq3, Inverted::kRightQuarterBankedHelixLargeDownSeq4, Inverted::kRightQuarterBankedHelixLargeDownSeq5, Inverted::kRightQuarterBankedHelixLargeDownSeq6 }}, // RightQuarterBankedHelixLargeDown
        {{ Inverted::kLeftQuarterHelixLargeUpSeq0, Inverted::kLeftQuarterHelixLargeUpSeq1, Inverted::kLeftQuarterHelixLargeUpSeq2, Inverted::kLeftQuarterHelixLargeUpSeq3, Inverted::kLeftQuarterHelixLargeUpSeq4, Inverted::kLeftQuarterHelixLargeUpSeq5, Inverted::kLeftQuarterHelixLargeUpSeq6 }}, // LeftQuarterHelixLargeUp
        {{ Inverted::kRightQuarterHelixLargeUpSeq0, Inverted::kRightQuarterHelixLargeUpSeq1, Inverted::kRightQuarterHelixLargeUpSeq2, Inverted::kRightQuarterHelixLargeUpSeq3, Inverted::kRightQuarterHelixLargeUpSeq4, Inverted::kRightQuarterHelixLargeUpSeq5, Inverted::kRightQuarterHelixLargeUpSeq6 }}, // RightQuarterHelixLargeUp
        {{ Inverted::kLeftQuarterHelixLargeDownSeq0, Inverted::kLeftQuarterHelixLargeDownSeq1, Inverted::kLeftQuarterHelixLargeDownSeq2, Inverted::kLeftQuarterHelixLargeDownSeq3, Inverted::kLeftQuarterHelixLargeDownSeq4, Inverted::kLeftQuarterHelixLargeDownSeq5, Inverted::kLeftQuarterHelixLargeDownSeq6 }}, // LeftQuarterHelixLargeDown
        {{ Inverted::kRightQuarterHelixLargeDownSeq0, Inverted::kRightQuarterHelixLargeDownSeq1, Inverted::kRightQuarterHelixLargeDownSeq2, Inverted::kRightQuarterHelixLargeDownSeq3, Inverted::kRightQuarterHelixLargeDownSeq4, Inverted::kRightQuarterHelixLargeDownSeq5, Inverted::kRightQuarterHelixLargeDownSeq6 }}, // RightQuarterHelixLargeDown
        {{ Inverted::kUp25LeftBankedSeq0 }}, // Up25LeftBanked
        {{ Inverted::kUp25RightBankedSeq0 }}, // Up25RightBanked
        {{ Inverted::kWaterfallSeq0 }}, // Waterfall
        {{ Inverted::kRapidsSeq0 }}, // Rapids
        {{ Inverted::kOnRidePhotoSeq0 }}, // OnRidePhoto
        {{ Inverted::kDown25LeftBankedSeq0 }}, // Down25LeftBanked
        {{ Inverted::kDown25RightBankedSeq0 }}, // Down25RightBanked
        {{ Inverted::kWatersplashSeq0, Inverted::kWatersplashSeq1, Inverted::kWatersplashSeq2, Inverted::kWatersplashSeq3, Inverted::kWatersplashSeq4 }}, // Watersplash
        {{ Inverted::kFlatToUp60LongBaseSeq0, Inverted::kFlatToUp60LongBaseSeq1, Inverted::kFlatToUp60LongBaseSeq2, Inverted::kFlatToUp60LongBaseSeq3 }}, // FlatToUp60LongBase
        {{ Inverted::kUp60ToFlatLongBaseSeq0, Inverted::kUp60ToFlatLongBaseSeq1, Inverted::kUp60ToFlatLongBaseSeq2, Inverted::kUp60ToFlatLongBaseSeq3 }}, // Up60ToFlatLongBase
        {{ Inverted::kWhirlpoolSeq0 }}, // Whirlpool
        {{ Inverted::kDown60ToFlatLongBaseSeq0, Inverted::kDown60ToFlatLongBaseSeq1, Inverted::kDown60ToFlatLongBaseSeq2, Inverted::kDown60ToFlatLongBaseSeq3 }}, // Down60ToFlatLongBase
        {{ Inverted::kFlatToDown60LongBaseSeq0, Inverted::kFlatToDown60LongBaseSeq1, Inverted::kFlatToDown60LongBaseSeq2, Inverted::kFlatToDown60LongBaseSeq3 }}, // FlatToDown60LongBase
        {{ Inverted::kCableLiftHillSeq0, Inverted::kCableLiftHillSeq1, Inverted::kCableLiftHillSeq2, Inverted::kCableLiftHillSeq3 }}, // CableLiftHill
        {{ Inverted::kReverseFreefallSlopeSeq0, Inverted::kReverseFreefallSlopeSeq1, Inverted::kReverseFreefallSlopeSeq2, Inverted::kReverseFreefallSlopeSeq3, Inverted::kReverseFreefallSlopeSeq4, Inverted::kReverseFreefallSlopeSeq5, Inverted::kReverseFreefallSlopeSeq6 }}, // ReverseFreefallSlope
        {{ Inverted::kReverseFreefallVerticalSeq0, Inverted::kReverseFreefallVerticalSeq1 }}, // ReverseFreefallVertical
        {{ Inverted::kUp90Seq0, Inverted::kUp90Seq1 }}, // Up90
        {{ Inverted::kDown90Seq0, Inverted::kDown90Seq1 }}, // Down90
        {{ Inverted::kUp60ToUp90Seq0, Inverted::kUp60ToUp90Seq1 }}, // Up60ToUp90
        {{ Inverted::kDown90ToDown60Seq0 }}, // Down90ToDown60
        {{ Inverted::kUp90ToUp60Seq0 }}, // Up90ToUp60
        {{ Inverted::kDown60ToDown90Seq0, Inverted::kDown60ToDown90Seq1 }}, // Down60ToDown90
        {{ Inverted::kBrakeForDropSeq0 }}, // BrakeForDrop
        {{ SuspendedSwingingTrain::kLeftEighthToDiagSeq0, Inverted::kLeftEighthToDiagSeq1, Inverted::kLeftEighthToDiagSeq2, Inverted::kLeftEighthToDiagSeq3, SuspendedSwingingTrain::kLeftEighthToDiagSeq4 }}, // LeftEighthToDiag
        {{ SuspendedSwingingTrain::kRightEighthToDiagSeq0, Inverted::kRightEighthToDiagSeq1, Inverted::kRightEighthToDiagSeq2, Inverted::kRightEighthToDiagSeq3, SuspendedSwingingTrain::kRightEighthToDiagSeq4 }}, // RightEighthToDiag
        {{ SuspendedSwingingTrain::kLeftEighthToOrthogonalSeq0, Inverted::kLeftEighthToOrthogonalSeq1, Inverted::kLeftEighthToOrthogonalSeq2, Inverted::kLeftEighthToOrthogonalSeq3, SuspendedSwingingTrain::kLeftEighthToOrthogonalSeq4 }}, // LeftEighthToOrthogonal
        {{ SuspendedSwingingTrain::kRightEighthToOrthogonalSeq0, Inverted::kRightEighthToOrthogonalSeq1, Inverted::kRightEighthToOrthogonalSeq2, Inverted::kRightEighthToOrthogonalSeq3, SuspendedSwingingTrain::kRightEighthToOrthogonalSeq4 }}, // RightEighthToOrthogonal
        {{ Inverted::kLeftEighthBankToDiagSeq0, Inverted::kLeftEighthBankToDiagSeq1, Inverted::kLeftEighthBankToDiagSeq2, Inverted::kLeftEighthBankToDiagSeq3, Inverted::kLeftEighthBankToDiagSeq4 }}, // LeftEighthBankToDiag
        {{ Inverted::kRightEighthBankToDiagSeq0, Inverted::kRightEighthBankToDiagSeq1, Inverted::kRightEighthBankToDiagSeq2, Inverted::kRightEighthBankToDiagSeq3, Inverted::kRightEighthBankToDiagSeq4 }}, // RightEighthBankToDiag
        {{ Inverted::kLeftEighthBankToOrthogonalSeq0, Inverted::kLeftEighthBankToOrthogonalSeq1, Inverted::kLeftEighthBankToOrthogonalSeq2, Inverted::kLeftEighthBankToOrthogonalSeq3, Inverted::kLeftEighthBankToOrthogonalSeq4 }}, // LeftEighthBankToOrthogonal
        {{ Inverted::kRightEighthBankToOrthogonalSeq0, Inverted::kRightEighthBankToOrthogonalSeq1, Inverted::kRightEighthBankToOrthogonalSeq2, Inverted::kRightEighthBankToOrthogonalSeq3, Inverted::kRightEighthBankToOrthogonalSeq4 }}, // RightEighthBankToOrthogonal
        {{ Inverted::kDiagFlatSeq0, Inverted::kDiagFlatSeq1, Inverted::kDiagFlatSeq2, Inverted::kDiagFlatSeq3 }}, // DiagFlat
        {{ Inverted::kDiagUp25Seq0, Inverted::kDiagUp25Seq1, Inverted::kDiagUp25Seq2, Inverted::kDiagUp25Seq3 }}, // DiagUp25
        {{ Inverted::kDiagUp60Seq0, Inverted::kDiagUp60Seq1, Inverted::kDiagUp60Seq2, Inverted::kDiagUp60Seq3 }}, // DiagUp60
        {{ Inverted::kDiagFlatToUp25Seq0, Inverted::kDiagFlatToUp25Seq1, Inverted::kDiagFlatToUp25Seq2, Inverted::kDiagFlatToUp25Seq3 }}, // DiagFlatToUp25
        {{ Inverted::kDiagUp25ToUp60Seq0, Inverted::kDiagUp25ToUp60Seq1, Inverted::kDiagUp25ToUp60Seq2, Inverted::kDiagUp25ToUp60Seq3 }}, // DiagUp25ToUp60
        {{ Inverted::kDiagUp60ToUp25Seq0, Inverted::kDiagUp60ToUp25Seq1, Inverted::kDiagUp60ToUp25Seq2, Inverted::kDiagUp60ToUp25Seq3 }}, // DiagUp60ToUp25
        {{ Inverted::kDiagUp25ToFlatSeq0, Inverted::kDiagUp25ToFlatSeq1, Inverted::kDiagUp25ToFlatSeq2, Inverted::kDiagUp25ToFlatSeq3 }}, // DiagUp25ToFlat
        {{ Inverted::kDiagDown25Seq0, Inverted::kDiagDown25Seq1, Inverted::kDiagDown25Seq2, Inverted::kDiagDown25Seq3 }}, // DiagDown25
        {{ Inverted::kDiagDown60Seq0, Inverted::kDiagDown60Seq1, Inverted::kDiagDown60Seq2, Inverted::kDiagDown60Seq3 }}, // DiagDown60
        {{ Inverted::kDiagFlatToDown25Seq0, Inverted::kDiagFlatToDown25Seq1, Inverted::kDiagFlatToDown25Seq2, Inverted::kDiagFlatToDown25Seq3 }}, // DiagFlatToDown25
        {{ Inverted::kDiagDown25ToDown60Seq0, Inverted::kDiagDown25ToDown60Seq1, Inverted::kDiagDown25ToDown60Seq2, Inverted::kDiagDown25ToDown60Seq3 }}, // DiagDown25ToDown60
        {{ Inverted::kDiagDown60ToDown25Seq0, Inverted::kDiagDown60ToDown25Seq1, Inverted::kDiagDown60ToDown25Seq2, Inverted::kDiagDown60ToDown25Seq3 }}, // DiagDown60ToDown25
        {{ Inverted::kDiagDown25ToFlatSeq0, Inverted::kDiagDown25ToFlatSeq1, Inverted::kDiagDown25ToFlatSeq2, Inverted::kDiagDown25ToFlatSeq3 }}, // DiagDown25ToFlat
        {{ Inverted::kDiagFlatToUp60Seq0, Inverted::kDiagFlatToUp60Seq1, Inverted::kDiagFlatToUp60Seq2, Inverted::kDiagFlatToUp60Seq3 }}, // DiagFlatToUp60
        {{ Inverted::kDiagUp60ToFlatSeq0, Inverted::kDiagUp60ToFlatSeq1, Inverted::kDiagUp60ToFlatSeq2, Inverted::kDiagUp60ToFlatSeq3 }}, // DiagUp60ToFlat
        {{ Inverted::kDiagFlatToDown60Seq0, Inverted::kDiagFlatToDown60Seq1, Inverted::kDiagFlatToDown60Seq2, Inverted::kDiagFlatToDown60Seq3 }}, // DiagFlatToDown60
        {{ Inverted::kDiagDown60ToFlatSeq0, Inverted::kDiagDown60ToFlatSeq1, Inverted::kDiagDown60ToFlatSeq2, Inverted::kDiagDown60ToFlatSeq3 }}, // DiagDown60ToFlat
        {{ Inverted::kDiagFlatToLeftBankSeq0, Inverted::kDiagFlatToLeftBankSeq1, Inverted::kDiagFlatToLeftBankSeq2, Inverted::kDiagFlatToLeftBankSeq3 }}, // DiagFlatToLeftBank
        {{ Inverted::kDiagFlatToRightBankSeq0, Inverted::kDiagFlatToRightBankSeq1, Inverted::kDiagFlatToRightBankSeq2, Inverted::kDiagFlatToRightBankSeq3 }}, // DiagFlatToRightBank
        {{ Inverted::kDiagLeftBankToFlatSeq0, Inverted::kDiagLeftBankToFlatSeq1, Inverted::kDiagLeftBankToFlatSeq2, Inverted::kDiagLeftBankToFlatSeq3 }}, // DiagLeftBankToFlat
        {{ Inverted::kDiagRightBankToFlatSeq0, Inverted::kDiagRightBankToFlatSeq1, Inverted::kDiagRightBankToFlatSeq2, Inverted::kDiagRightBankToFlatSeq3 }}, // DiagRightBankToFlat
        {{ Inverted::kDiagLeftBankToUp25Seq0, Inverted::kDiagLeftBankToUp25Seq1, Inverted::kDiagLeftBankToUp25Seq2, Inverted::kDiagLeftBankToUp25Seq3 }}, // DiagLeftBankToUp25
        {{ Inverted::kDiagRightBankToUp25Seq0, Inverted::kDiagRightBankToUp25Seq1, Inverted::kDiagRightBankToUp25Seq2, Inverted::kDiagRightBankToUp25Seq3 }}, // DiagRightBankToUp25
        {{ Inverted::kDiagUp25ToLeftBankSeq0, Inverted::kDiagUp25ToLeftBankSeq1, Inverted::kDiagUp25ToLeftBankSeq2, Inverted::kDiagUp25ToLeftBankSeq3 }}, // DiagUp25ToLeftBank
        {{ Inverted::kDiagUp25ToRightBankSeq0, Inverted::kDiagUp25ToRightBankSeq1, Inverted::kDiagUp25ToRightBankSeq2, Inverted::kDiagUp25ToRightBankSeq3 }}, // DiagUp25ToRightBank
        {{ Inverted::kDiagLeftBankToDown25Seq0, Inverted::kDiagLeftBankToDown25Seq1, Inverted::kDiagLeftBankToDown25Seq2, Inverted::kDiagLeftBankToDown25Seq3 }}, // DiagLeftBankToDown25
        {{ Inverted::kDiagRightBankToDown25Seq0, Inverted::kDiagRightBankToDown25Seq1, Inverted::kDiagRightBankToDown25Seq2, Inverted::kDiagRightBankToDown25Seq3 }}, // DiagRightBankToDown25
        {{ Inverted::kDiagDown25ToLeftBankSeq0, Inverted::kDiagDown25ToLeftBankSeq1, Inverted::kDiagDown25ToLeftBankSeq2, Inverted::kDiagDown25ToLeftBankSeq3 }}, // DiagDown25ToLeftBank
        {{ Inverted::kDiagDown25ToRightBankSeq0, Inverted::kDiagDown25ToRightBankSeq1, Inverted::kDiagDown25ToRightBankSeq2, Inverted::kDiagDown25ToRightBankSeq3 }}, // DiagDown25ToRightBank
        {{ Inverted::kDiagLeftBankSeq0, Inverted::kDiagLeftBankSeq1, Inverted::kDiagLeftBankSeq2, Inverted::kDiagLeftBankSeq3 }}, // DiagLeftBank
        {{ Inverted::kDiagRightBankSeq0, Inverted::kDiagRightBankSeq1, Inverted::kDiagRightBankSeq2, Inverted::kDiagRightBankSeq3 }}, // DiagRightBank
        {{ Inverted::kLogFlumeReverserSeq0 }}, // LogFlumeReverser
        {{ Inverted::kSpinningTunnelSeq0 }}, // SpinningTunnel
        {{ Inverted::kLeftBarrelRollUpToDownSeq0, Inverted::kLeftBarrelRollUpToDownSeq1, Inverted::kLeftBarrelRollUpToDownSeq2 }}, // LeftBarrelRollUpToDown
        {{ Inverted::kRightBarrelRollUpToDownSeq0, Inverted::kRightBarrelRollUpToDownSeq1, Inverted::kRightBarrelRollUpToDownSeq2 }}, // RightBarrelRollUpToDown
        {{ Inverted::kLeftBarrelRollDownToUpSeq0, Inverted::kLeftBarrelRollDownToUpSeq1, Inverted::kLeftBarrelRollDownToUpSeq2 }}, // LeftBarrelRollDownToUp
        {{ Inverted::kRightBarrelRollDownToUpSeq0, Inverted::kRightBarrelRollDownToUpSeq1, Inverted::kRightBarrelRollDownToUpSeq2 }}, // RightBarrelRollDownToUp
        {{ Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq0, Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq1, Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq2, Inverted::kLeftBankToLeftQuarterTurn3TilesUp25Seq3 }}, // LeftBankToLeftQuarterTurn3TilesUp25
        {{ Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq0, Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq1, Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq2, Inverted::kRightBankToRightQuarterTurn3TilesUp25Seq3 }}, // RightBankToRightQuarterTurn3TilesUp25
        {{ Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq1, Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, Inverted::kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 }}, // LeftQuarterTurn3TilesDown25ToLeftBank
        {{ Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq0, Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq1, Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq2, Inverted::kRightQuarterTurn3TilesDown25ToRightBankSeq3 }}, // RightQuarterTurn3TilesDown25ToRightBank
        {{ Inverted::kPoweredLiftSeq0 }}, // PoweredLift
        {{ Inverted::kLeftLargeHalfLoopUpSeq0, Inverted::kLeftLargeHalfLoopUpSeq1, Inverted::kLeftLargeHalfLoopUpSeq2, Inverted::kLeftLargeHalfLoopUpSeq3, Inverted::kLeftLargeHalfLoopUpSeq4, Inverted::kLeftLargeHalfLoopUpSeq5, Inverted::kLeftLargeHalfLoopUpSeq6 }}, // LeftLargeHalfLoopUp
        {{ Inverted::kRightLargeHalfLoopUpSeq0, Inverted::kRightLargeHalfLoopUpSeq1, Inverted::kRightLargeHalfLoopUpSeq2, Inverted::kRightLargeHalfLoopUpSeq3, Inverted::kRightLargeHalfLoopUpSeq4, Inverted::kRightLargeHalfLoopUpSeq5, Inverted::kRightLargeHalfLoopUpSeq6 }}, // RightLargeHalfLoopUp
        {{ Inverted::kLeftLargeHalfLoopDownSeq0, Inverted::kLeftLargeHalfLoopDownSeq1, Inverted::kLeftLargeHalfLoopDownSeq2, Inverted::kLeftLargeHalfLoopDownSeq3, Inverted::kLeftLargeHalfLoopDownSeq4, Inverted::kLeftLargeHalfLoopDownSeq5, Inverted::kLeftLargeHalfLoopDownSeq6 }}, // LeftLargeHalfLoopDown
        {{ Inverted::kRightLargeHalfLoopDownSeq0, Inverted::kRightLargeHalfLoopDownSeq1, Inverted::kRightLargeHalfLoopDownSeq2, Inverted::kRightLargeHalfLoopDownSeq3, Inverted::kRightLargeHalfLoopDownSeq4, Inverted::kRightLargeHalfLoopDownSeq5, Inverted::kRightLargeHalfLoopDownSeq6 }}, // RightLargeHalfLoopDown
        {{ Inverted::kLeftFlyerTwistUpSeq0, Inverted::kLeftFlyerTwistUpSeq1, Inverted::kLeftFlyerTwistUpSeq2 }}, // LeftFlyerTwistUp
        {{ Inverted::kRightFlyerTwistUpSeq0, Inverted::kRightFlyerTwistUpSeq1, Inverted::kRightFlyerTwistUpSeq2 }}, // RightFlyerTwistUp
        {{ Inverted::kLeftFlyerTwistDownSeq0, Inverted::kLeftFlyerTwistDownSeq1, Inverted::kLeftFlyerTwistDownSeq2 }}, // LeftFlyerTwistDown
        {{ Inverted::kRightFlyerTwistDownSeq0, Inverted::kRightFlyerTwistDownSeq1, Inverted::kRightFlyerTwistDownSeq2 }}, // RightFlyerTwistDown
        {{ Inverted::kFlyerHalfLoopUninvertedUpSeq0, Inverted::kFlyerHalfLoopUninvertedUpSeq1, Inverted::kFlyerHalfLoopUninvertedUpSeq2, Inverted::kFlyerHalfLoopUninvertedUpSeq3 }}, // FlyerHalfLoopUninvertedUp
        {{ Inverted::kFlyerHalfLoopInvertedDownSeq0, Inverted::kFlyerHalfLoopInvertedDownSeq1, Inverted::kFlyerHalfLoopInvertedDownSeq2, Inverted::kFlyerHalfLoopInvertedDownSeq3 }}, // FlyerHalfLoopInvertedDown
        {{ Inverted::kLeftFlyerCorkscrewUpSeq0, Inverted::kLeftFlyerCorkscrewUpSeq1, Inverted::kLeftFlyerCorkscrewUpSeq2 }}, // LeftFlyerCorkscrewUp
        {{ Inverted::kRightFlyerCorkscrewUpSeq0, Inverted::kRightFlyerCorkscrewUpSeq1, Inverted::kRightFlyerCorkscrewUpSeq2 }}, // RightFlyerCorkscrewUp
        {{ Inverted::kLeftFlyerCorkscrewDownSeq0, Inverted::kLeftFlyerCorkscrewDownSeq1, Inverted::kLeftFlyerCorkscrewDownSeq2 }}, // LeftFlyerCorkscrewDown
        {{ Inverted::kRightFlyerCorkscrewDownSeq0, Inverted::kRightFlyerCorkscrewDownSeq1, Inverted::kRightFlyerCorkscrewDownSeq2 }}, // RightFlyerCorkscrewDown
        {{ Inverted::kHeartLineTransferUpSeq0, Inverted::kHeartLineTransferUpSeq1, Inverted::kHeartLineTransferUpSeq2, Inverted::kHeartLineTransferUpSeq3 }}, // HeartLineTransferUp
        {{ Inverted::kHeartLineTransferDownSeq0, Inverted::kHeartLineTransferDownSeq1, Inverted::kHeartLineTransferDownSeq2, Inverted::kHeartLineTransferDownSeq3 }}, // HeartLineTransferDown
        {{ Inverted::kLeftHeartLineRollSeq0, Inverted::kLeftHeartLineRollSeq1, Inverted::kLeftHeartLineRollSeq2, Inverted::kLeftHeartLineRollSeq3, Inverted::kLeftHeartLineRollSeq4, Inverted::kLeftHeartLineRollSeq5 }}, // LeftHeartLineRoll
        {{ Inverted::kRightHeartLineRollSeq0, Inverted::kRightHeartLineRollSeq1, Inverted::kRightHeartLineRollSeq2, Inverted::kRightHeartLineRollSeq3, Inverted::kRightHeartLineRollSeq4, Inverted::kRightHeartLineRollSeq5 }}, // RightHeartLineRoll
        {{ Inverted::kMinigolfHoleASeq0, Inverted::kMinigolfHoleASeq1 }}, // MinigolfHoleA
        {{ Inverted::kMinigolfHoleBSeq0, Inverted::kMinigolfHoleBSeq1 }}, // MinigolfHoleB
        {{ Inverted::kMinigolfHoleCSeq0, Inverted::kMinigolfHoleCSeq1 }}, // MinigolfHoleC
        {{ Inverted::kMinigolfHoleDSeq0, Inverted::kMinigolfHoleDSeq1, Inverted::kMinigolfHoleDSeq2 }}, // MinigolfHoleD
        {{ Inverted::kMinigolfHoleESeq0, Inverted::kMinigolfHoleESeq1, Inverted::kMinigolfHoleESeq2 }}, // MinigolfHoleE
        {{ Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq0, Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq1, Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq2, Inverted::kMultiDimInvertedFlatToDown90QuarterLoopSeq3 }}, // MultiDimInvertedFlatToDown90QuarterLoop
        {{ Inverted::kUp90ToInvertedFlatQuarterLoopSeq0, Inverted::kUp90ToInvertedFlatQuarterLoopSeq1, Inverted::kUp90ToInvertedFlatQuarterLoopSeq2 }}, // Up90ToInvertedFlatQuarterLoop
        {{ Inverted::kInvertedFlatToDown90QuarterLoopSeq0, Inverted::kInvertedFlatToDown90QuarterLoopSeq1, Inverted::kInvertedFlatToDown90QuarterLoopSeq2, Inverted::kInvertedFlatToDown90QuarterLoopSeq3 }}, // InvertedFlatToDown90QuarterLoop
        {{ Inverted::kLeftCurvedLiftHillSeq0, Inverted::kLeftCurvedLiftHillSeq1, Inverted::kLeftCurvedLiftHillSeq2, Inverted::kLeftCurvedLiftHillSeq3 }}, // LeftCurvedLiftHill
        {{ Inverted::kRightCurvedLiftHillSeq0, Inverted::kRightCurvedLiftHillSeq1, Inverted::kRightCurvedLiftHillSeq2, Inverted::kRightCurvedLiftHillSeq3 }}, // RightCurvedLiftHill
        {{ Inverted::kLeftReverserSeq0, Inverted::kLeftReverserSeq1, Inverted::kLeftReverserSeq2, Inverted::kLeftReverserSeq3, Inverted::kLeftReverserSeq4, Inverted::kLeftReverserSeq5 }}, // LeftReverser
        {{ Inverted::kRightReverserSeq0, Inverted::kRightReverserSeq1, Inverted::kRightReverserSeq2, Inverted::kRightReverserSeq3, Inverted::kRightReverserSeq4, Inverted::kRightReverserSeq5 }}, // RightReverser
        {{ Inverted::kAirThrustTopCapSeq0, Inverted::kAirThrustTopCapSeq1, Inverted::kAirThrustTopCapSeq2, Inverted::kAirThrustTopCapSeq3 }}, // AirThrustTopCap
        {{ Inverted::kAirThrustVerticalDownSeq0, Inverted::kAirThrustVerticalDownSeq1 }}, // AirThrustVerticalDown
        {{ Inverted::kAirThrustVerticalDownToLevelSeq0, Inverted::kAirThrustVerticalDownToLevelSeq1, Inverted::kAirThrustVerticalDownToLevelSeq2, Inverted::kAirThrustVerticalDownToLevelSeq3, Inverted::kAirThrustVerticalDownToLevelSeq4, Inverted::kAirThrustVerticalDownToLevelSeq5, Inverted::kAirThrustVerticalDownToLevelSeq6 }}, // AirThrustVerticalDownToLevel
        {{ Inverted::kBlockBrakesSeq0 }}, // BlockBrakes
        {{ Inverted::kLeftBankedQuarterTurn3TileUp25Seq0, Inverted::kLeftBankedQuarterTurn3TileUp25Seq1, Inverted::kLeftBankedQuarterTurn3TileUp25Seq2, Inverted::kLeftBankedQuarterTurn3TileUp25Seq3 }}, // LeftBankedQuarterTurn3TileUp25
        {{ Inverted::kRightBankedQuarterTurn3TileUp25Seq0, Inverted::kRightBankedQuarterTurn3TileUp25Seq1, Inverted::kRightBankedQuarterTurn3TileUp25Seq2, Inverted::kRightBankedQuarterTurn3TileUp25Seq3 }}, // RightBankedQuarterTurn3TileUp25
        {{ Inverted::kLeftBankedQuarterTurn3TileDown25Seq0, Inverted::kLeftBankedQuarterTurn3TileDown25Seq1, Inverted::kLeftBankedQuarterTurn3TileDown25Seq2, Inverted::kLeftBankedQuarterTurn3TileDown25Seq3 }}, // LeftBankedQuarterTurn3TileDown25
        {{ Inverted::kRightBankedQuarterTurn3TileDown25Seq0, Inverted::kRightBankedQuarterTurn3TileDown25Seq1, Inverted::kRightBankedQuarterTurn3TileDown25Seq2, Inverted::kRightBankedQuarterTurn3TileDown25Seq3 }}, // RightBankedQuarterTurn3TileDown25
        {{ Inverted::kLeftBankedQuarterTurn5TileUp25Seq0, Inverted::kLeftBankedQuarterTurn5TileUp25Seq1, Inverted::kLeftBankedQuarterTurn5TileUp25Seq2, Inverted::kLeftBankedQuarterTurn5TileUp25Seq3, Inverted::kLeftBankedQuarterTurn5TileUp25Seq4, Inverted::kLeftBankedQuarterTurn5TileUp25Seq5, Inverted::kLeftBankedQuarterTurn5TileUp25Seq6 }}, // LeftBankedQuarterTurn5TileUp25
        {{ Inverted::kRightBankedQuarterTurn5TileUp25Seq0, Inverted::kRightBankedQuarterTurn5TileUp25Seq1, Inverted::kRightBankedQuarterTurn5TileUp25Seq2, Inverted::kRightBankedQuarterTurn5TileUp25Seq3, Inverted::kRightBankedQuarterTurn5TileUp25Seq4, Inverted::kRightBankedQuarterTurn5TileUp25Seq5, Inverted::kRightBankedQuarterTurn5TileUp25Seq6 }}, // RightBankedQuarterTurn5TileUp25
        {{ Inverted::kLeftBankedQuarterTurn5TileDown25Seq0, Inverted::kLeftBankedQuarterTurn5TileDown25Seq1, Inverted::kLeftBankedQuarterTurn5TileDown25Seq2, Inverted::kLeftBankedQuarterTurn5TileDown25Seq3, Inverted::kLeftBankedQuarterTurn5TileDown25Seq4, Inverted::kLeftBankedQuarterTurn5TileDown25Seq5, Inverted::kLeftBankedQuarterTurn5TileDown25Seq6 }}, // LeftBankedQuarterTurn5TileDown25
        {{ Inverted::kRightBankedQuarterTurn5TileDown25Seq0, Inverted::kRightBankedQuarterTurn5TileDown25Seq1, Inverted::kRightBankedQuarterTurn5TileDown25Seq2, Inverted::kRightBankedQuarterTurn5TileDown25Seq3, Inverted::kRightBankedQuarterTurn5TileDown25Seq4, Inverted::kRightBankedQuarterTurn5TileDown25Seq5, Inverted::kRightBankedQuarterTurn5TileDown25Seq6 }}, // RightBankedQuarterTurn5TileDown25
        {{ Inverted::kUp25ToLeftBankedUp25Seq0 }}, // Up25ToLeftBankedUp25
        {{ Inverted::kUp25ToRightBankedUp25Seq0 }}, // Up25ToRightBankedUp25
        {{ Inverted::kLeftBankedUp25ToUp25Seq0 }}, // LeftBankedUp25ToUp25
        {{ Inverted::kRightBankedUp25ToUp25Seq0 }}, // RightBankedUp25ToUp25
        {{ Inverted::kDown25ToLeftBankedDown25Seq0 }}, // Down25ToLeftBankedDown25
        {{ Inverted::kDown25ToRightBankedDown25Seq0 }}, // Down25ToRightBankedDown25
        {{ Inverted::kLeftBankedDown25ToDown25Seq0 }}, // LeftBankedDown25ToDown25
        {{ Inverted::kRightBankedDown25ToDown25Seq0 }}, // RightBankedDown25ToDown25
        {{ Inverted::kLeftBankedFlatToLeftBankedUp25Seq0 }}, // LeftBankedFlatToLeftBankedUp25
        {{ Inverted::kRightBankedFlatToRightBankedUp25Seq0 }}, // RightBankedFlatToRightBankedUp25
        {{ Inverted::kLeftBankedUp25ToLeftBankedFlatSeq0 }}, // LeftBankedUp25ToLeftBankedFlat
        {{ Inverted::kRightBankedUp25ToRightBankedFlatSeq0 }}, // RightBankedUp25ToRightBankedFlat
        {{ Inverted::kLeftBankedFlatToLeftBankedDown25Seq0 }}, // LeftBankedFlatToLeftBankedDown25
        {{ Inverted::kRightBankedFlatToRightBankedDown25Seq0 }}, // RightBankedFlatToRightBankedDown25
        {{ Inverted::kLeftBankedDown25ToLeftBankedFlatSeq0 }}, // LeftBankedDown25ToLeftBankedFlat
        {{ Inverted::kRightBankedDown25ToRightBankedFlatSeq0 }}, // RightBankedDown25ToRightBankedFlat
        {{ Inverted::kFlatToLeftBankedUp25Seq0 }}, // FlatToLeftBankedUp25
        {{ Inverted::kFlatToRightBankedUp25Seq0 }}, // FlatToRightBankedUp25
        {{ Inverted::kLeftBankedUp25ToFlatSeq0 }}, // LeftBankedUp25ToFlat
        {{ Inverted::kRightBankedUp25ToFlatSeq0 }}, // RightBankedUp25ToFlat
        {{ Inverted::kFlatToLeftBankedDown25Seq0 }}, // FlatToLeftBankedDown25
        {{ Inverted::kFlatToRightBankedDown25Seq0 }}, // FlatToRightBankedDown25
        {{ Inverted::kLeftBankedDown25ToFlatSeq0 }}, // LeftBankedDown25ToFlat
        {{ Inverted::kRightBankedDown25ToFlatSeq0 }}, // RightBankedDown25ToFlat
        {{ Inverted::kLeftQuarterTurn1TileUp90Seq0, Inverted::kLeftQuarterTurn1TileUp90Seq1 }}, // LeftQuarterTurn1TileUp90
        {{ Inverted::kRightQuarterTurn1TileUp90Seq0, Inverted::kRightQuarterTurn1TileUp90Seq1 }}, // RightQuarterTurn1TileUp90
        {{ Inverted::kLeftQuarterTurn1TileDown90Seq0, Inverted::kLeftQuarterTurn1TileDown90Seq1 }}, // LeftQuarterTurn1TileDown90
        {{ Inverted::kRightQuarterTurn1TileDown90Seq0, Inverted::kRightQuarterTurn1TileDown90Seq1 }}, // RightQuarterTurn1TileDown90
        {{ Inverted::kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, Inverted::kMultiDimUp90ToInvertedFlatQuarterLoopSeq1, Inverted::kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 }}, // MultiDimUp90ToInvertedFlatQuarterLoop
        {{ Inverted::kMultiDimFlatToDown90QuarterLoopSeq0, Inverted::kMultiDimFlatToDown90QuarterLoopSeq1, Inverted::kMultiDimFlatToDown90QuarterLoopSeq2, Inverted::kMultiDimFlatToDown90QuarterLoopSeq3 }}, // MultiDimFlatToDown90QuarterLoop
        {{ Inverted::kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, Inverted::kMultiDimInvertedUp90ToFlatQuarterLoopSeq1, Inverted::kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 }}, // MultiDimInvertedUp90ToFlatQuarterLoop
        {{ Inverted::kRotationControlToggleSeq0 }}, // RotationControlToggle
        {{ Inverted::kFlatTrack1x4ASeq0, Inverted::kFlatTrack1x4ASeq1, Inverted::kFlatTrack1x4ASeq2, Inverted::kFlatTrack1x4ASeq3 }}, // FlatTrack1x4A
        {{ Inverted::kFlatTrack2x2Seq0, Inverted::kFlatTrack2x2Seq1, Inverted::kFlatTrack2x2Seq2, Inverted::kFlatTrack2x2Seq3 }}, // FlatTrack2x2
        {{ Inverted::kFlatTrack4x4Seq0, Inverted::kFlatTrack4x4Seq1, Inverted::kFlatTrack4x4Seq2, Inverted::kFlatTrack4x4Seq3, Inverted::kFlatTrack4x4Seq4, Inverted::kFlatTrack4x4Seq5, Inverted::kFlatTrack4x4Seq6, Inverted::kFlatTrack4x4Seq7, Inverted::kFlatTrack4x4Seq8, Inverted::kFlatTrack4x4Seq9, Inverted::kFlatTrack4x4Seq10, Inverted::kFlatTrack4x4Seq11, Inverted::kFlatTrack4x4Seq12, Inverted::kFlatTrack4x4Seq13, Inverted::kFlatTrack4x4Seq14, Inverted::kFlatTrack4x4Seq15 }}, // FlatTrack4x4
        {{ Inverted::kFlatTrack2x4Seq0, Inverted::kFlatTrack2x4Seq1, Inverted::kFlatTrack2x4Seq2, Inverted::kFlatTrack2x4Seq3, Inverted::kFlatTrack2x4Seq4, Inverted::kFlatTrack2x4Seq5, Inverted::kFlatTrack2x4Seq6, Inverted::kFlatTrack2x4Seq7 }}, // FlatTrack2x4
        {{ Inverted::kFlatTrack1x5Seq0, Inverted::kFlatTrack1x5Seq1, Inverted::kFlatTrack1x5Seq2, Inverted::kFlatTrack1x5Seq3, Inverted::kFlatTrack1x5Seq4 }}, // FlatTrack1x5
        {{ Inverted::kFlatTrack1x1ASeq0 }}, // FlatTrack1x1A
        {{ Inverted::kFlatTrack1x4BSeq0, Inverted::kFlatTrack1x4BSeq1, Inverted::kFlatTrack1x4BSeq2, Inverted::kFlatTrack1x4BSeq3 }}, // FlatTrack1x4B
        {{ Inverted::kFlatTrack1x1BSeq0 }}, // FlatTrack1x1B
        {{ Inverted::kFlatTrack1x4CSeq0, Inverted::kFlatTrack1x4CSeq1, Inverted::kFlatTrack1x4CSeq2, Inverted::kFlatTrack1x4CSeq3 }}, // FlatTrack1x4C
        {{ Inverted::kFlatTrack3x3Seq0, Inverted::kFlatTrack3x3Seq1, Inverted::kFlatTrack3x3Seq2, Inverted::kFlatTrack3x3Seq3, Inverted::kFlatTrack3x3Seq4, Inverted::kFlatTrack3x3Seq5, Inverted::kFlatTrack3x3Seq6, Inverted::kFlatTrack3x3Seq7, Inverted::kFlatTrack3x3Seq8 }}, // FlatTrack3x3
        {{ Inverted::kLeftLargeCorkscrewUpSeq0, Inverted::kLeftLargeCorkscrewUpSeq1, Inverted::kLeftLargeCorkscrewUpSeq2, Inverted::kLeftLargeCorkscrewUpSeq3, Inverted::kLeftLargeCorkscrewUpSeq4, Inverted::kLeftLargeCorkscrewUpSeq5 }}, // LeftLargeCorkscrewUp
        {{ Inverted::kRightLargeCorkscrewUpSeq0, Inverted::kRightLargeCorkscrewUpSeq1, Inverted::kRightLargeCorkscrewUpSeq2, Inverted::kRightLargeCorkscrewUpSeq3, Inverted::kRightLargeCorkscrewUpSeq4, Inverted::kRightLargeCorkscrewUpSeq5 }}, // RightLargeCorkscrewUp
        {{ Inverted::kLeftLargeCorkscrewDownSeq0, Inverted::kLeftLargeCorkscrewDownSeq1, Inverted::kLeftLargeCorkscrewDownSeq2, Inverted::kLeftLargeCorkscrewDownSeq3, Inverted::kLeftLargeCorkscrewDownSeq4, Inverted::kLeftLargeCorkscrewDownSeq5 }}, // LeftLargeCorkscrewDown
        {{ Inverted::kRightLargeCorkscrewDownSeq0, Inverted::kRightLargeCorkscrewDownSeq1, Inverted::kRightLargeCorkscrewDownSeq2, Inverted::kRightLargeCorkscrewDownSeq3, Inverted::kRightLargeCorkscrewDownSeq4, Inverted::kRightLargeCorkscrewDownSeq5 }}, // RightLargeCorkscrewDown
        {{ Inverted::kLeftMediumHalfLoopUpSeq0, Inverted::kLeftMediumHalfLoopUpSeq1, Inverted::kLeftMediumHalfLoopUpSeq2, Inverted::kLeftMediumHalfLoopUpSeq3, Inverted::kLeftMediumHalfLoopUpSeq4 }}, // LeftMediumHalfLoopUp
        {{ Inverted::kRightMediumHalfLoopUpSeq0, Inverted::kRightMediumHalfLoopUpSeq1, Inverted::kRightMediumHalfLoopUpSeq2, Inverted::kRightMediumHalfLoopUpSeq3, Inverted::kRightMediumHalfLoopUpSeq4 }}, // RightMediumHalfLoopUp
        {{ Inverted::kLeftMediumHalfLoopDownSeq0, Inverted::kLeftMediumHalfLoopDownSeq1, Inverted::kLeftMediumHalfLoopDownSeq2, Inverted::kLeftMediumHalfLoopDownSeq3, Inverted::kLeftMediumHalfLoopDownSeq4 }}, // LeftMediumHalfLoopDown
        {{ Inverted::kRightMediumHalfLoopDownSeq0, Inverted::kRightMediumHalfLoopDownSeq1, Inverted::kRightMediumHalfLoopDownSeq2, Inverted::kRightMediumHalfLoopDownSeq3, Inverted::kRightMediumHalfLoopDownSeq4 }}, // RightMediumHalfLoopDown
        {{ Inverted::kLeftZeroGRollUpSeq0, Inverted::kLeftZeroGRollUpSeq1, Inverted::kLeftZeroGRollUpSeq2 }}, // LeftZeroGRollUp
        {{ Inverted::kRightZeroGRollUpSeq0, Inverted::kRightZeroGRollUpSeq1, Inverted::kRightZeroGRollUpSeq2 }}, // RightZeroGRollUp
        {{ Inverted::kLeftZeroGRollDownSeq0, Inverted::kLeftZeroGRollDownSeq1, Inverted::kLeftZeroGRollDownSeq2 }}, // LeftZeroGRollDown
        {{ Inverted::kRightZeroGRollDownSeq0, Inverted::kRightZeroGRollDownSeq1, Inverted::kRightZeroGRollDownSeq2 }}, // RightZeroGRollDown
        {{ Inverted::kLeftLargeZeroGRollUpSeq0, Inverted::kLeftLargeZeroGRollUpSeq1, Inverted::kLeftLargeZeroGRollUpSeq2, Inverted::kLeftLargeZeroGRollUpSeq3 }}, // LeftLargeZeroGRollUp
        {{ Inverted::kRightLargeZeroGRollUpSeq0, Inverted::kRightLargeZeroGRollUpSeq1, Inverted::kRightLargeZeroGRollUpSeq2, Inverted::kRightLargeZeroGRollUpSeq3 }}, // RightLargeZeroGRollUp
        {{ Inverted::kLeftLargeZeroGRollDownSeq0, Inverted::kLeftLargeZeroGRollDownSeq1, Inverted::kLeftLargeZeroGRollDownSeq2, Inverted::kLeftLargeZeroGRollDownSeq3 }}, // LeftLargeZeroGRollDown
        {{ Inverted::kRightLargeZeroGRollDownSeq0, Inverted::kRightLargeZeroGRollDownSeq1, Inverted::kRightLargeZeroGRollDownSeq2, Inverted::kRightLargeZeroGRollDownSeq3 }}, // RightLargeZeroGRollDown
        {{ Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq0, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq1, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq2, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq3, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq4, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq5, Inverted::kLeftFlyerLargeHalfLoopUninvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedUp
        {{ Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq0, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq1, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq2, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq3, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq4, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq5, Inverted::kRightFlyerLargeHalfLoopUninvertedUpSeq6 }}, // RightFlyerLargeHalfLoopUninvertedUp
        {{ Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq0, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq1, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq2, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq3, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq4, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq5, Inverted::kLeftFlyerLargeHalfLoopInvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopInvertedDown
        {{ Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq0, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq1, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq2, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq3, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq4, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq5, Inverted::kRightFlyerLargeHalfLoopInvertedDownSeq6 }}, // RightFlyerLargeHalfLoopInvertedDown
        {{ Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq0, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq1, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq2, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq3, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq4, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq5, Inverted::kLeftFlyerLargeHalfLoopInvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopInvertedUp
        {{ Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq0, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq1, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq2, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq3, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq4, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq5, Inverted::kRightFlyerLargeHalfLoopInvertedUpSeq6 }}, // RightFlyerLargeHalfLoopInvertedUp
        {{ Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq0, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq1, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq2, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq3, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq4, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq5, Inverted::kLeftFlyerLargeHalfLoopUninvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedDown
        {{ Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq0, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq1, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq2, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq3, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq4, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq5, Inverted::kRightFlyerLargeHalfLoopUninvertedDownSeq6 }}, // RightFlyerLargeHalfLoopUninvertedDown
        {{ Inverted::kFlyerHalfLoopInvertedUpSeq0, Inverted::kFlyerHalfLoopInvertedUpSeq1, Inverted::kFlyerHalfLoopInvertedUpSeq2, Inverted::kFlyerHalfLoopInvertedUpSeq3 }}, // FlyerHalfLoopInvertedUp
        {{ Inverted::kFlyerHalfLoopUninvertedDownSeq0, Inverted::kFlyerHalfLoopUninvertedDownSeq1, Inverted::kFlyerHalfLoopUninvertedDownSeq2, Inverted::kFlyerHalfLoopUninvertedDownSeq3 }}, // FlyerHalfLoopUninvertedDown
        {{ Inverted::kLeftEighthToDiagUp25Seq0, Inverted::kLeftEighthToDiagUp25Seq1, Inverted::kLeftEighthToDiagUp25Seq2, Inverted::kLeftEighthToDiagUp25Seq3, Inverted::kLeftEighthToDiagUp25Seq4 }}, // LeftEighthToDiagUp25
        {{ Inverted::kRightEighthToDiagUp25Seq0, Inverted::kRightEighthToDiagUp25Seq1, Inverted::kRightEighthToDiagUp25Seq2, Inverted::kRightEighthToDiagUp25Seq3, Inverted::kRightEighthToDiagUp25Seq4 }}, // RightEighthToDiagUp25
        {{ Inverted::kLeftEighthToDiagDown25Seq0, Inverted::kLeftEighthToDiagDown25Seq1, Inverted::kLeftEighthToDiagDown25Seq2, Inverted::kLeftEighthToDiagDown25Seq3, Inverted::kLeftEighthToDiagDown25Seq4 }}, // LeftEighthToDiagDown25
        {{ Inverted::kRightEighthToDiagDown25Seq0, Inverted::kRightEighthToDiagDown25Seq1, Inverted::kRightEighthToDiagDown25Seq2, Inverted::kRightEighthToDiagDown25Seq3, Inverted::kRightEighthToDiagDown25Seq4 }}, // RightEighthToDiagDown25
        {{ Inverted::kLeftEighthToOrthogonalUp25Seq0, Inverted::kLeftEighthToOrthogonalUp25Seq1, Inverted::kLeftEighthToOrthogonalUp25Seq2, Inverted::kLeftEighthToOrthogonalUp25Seq3, Inverted::kLeftEighthToOrthogonalUp25Seq4 }}, // LeftEighthToOrthogonalUp25
        {{ Inverted::kRightEighthToOrthogonalUp25Seq0, Inverted::kRightEighthToOrthogonalUp25Seq1, Inverted::kRightEighthToOrthogonalUp25Seq2, Inverted::kRightEighthToOrthogonalUp25Seq3, Inverted::kRightEighthToOrthogonalUp25Seq4 }}, // RightEighthToOrthogonalUp25
        {{ Inverted::kLeftEighthToOrthogonalDown25Seq0, Inverted::kLeftEighthToOrthogonalDown25Seq1, Inverted::kLeftEighthToOrthogonalDown25Seq2, Inverted::kLeftEighthToOrthogonalDown25Seq3, Inverted::kLeftEighthToOrthogonalDown25Seq4 }}, // LeftEighthToOrthogonalDown25
        {{ Inverted::kRightEighthToOrthogonalDown25Seq0, Inverted::kRightEighthToOrthogonalDown25Seq1, Inverted::kRightEighthToOrthogonalDown25Seq2, Inverted::kRightEighthToOrthogonalDown25Seq3, Inverted::kRightEighthToOrthogonalDown25Seq4 }}, // RightEighthToOrthogonalDown25
        {{ Inverted::kDiagUp25ToLeftBankedUp25Seq0, Inverted::kDiagUp25ToLeftBankedUp25Seq1, Inverted::kDiagUp25ToLeftBankedUp25Seq2, Inverted::kDiagUp25ToLeftBankedUp25Seq3 }}, // DiagUp25ToLeftBankedUp25
        {{ Inverted::kDiagUp25ToRightBankedUp25Seq0, Inverted::kDiagUp25ToRightBankedUp25Seq1, Inverted::kDiagUp25ToRightBankedUp25Seq2, Inverted::kDiagUp25ToRightBankedUp25Seq3 }}, // DiagUp25ToRightBankedUp25
        {{ Inverted::kDiagLeftBankedUp25ToUp25Seq0, Inverted::kDiagLeftBankedUp25ToUp25Seq1, Inverted::kDiagLeftBankedUp25ToUp25Seq2, Inverted::kDiagLeftBankedUp25ToUp25Seq3 }}, // DiagLeftBankedUp25ToUp25
        {{ Inverted::kDiagRightBankedUp25ToUp25Seq0, Inverted::kDiagRightBankedUp25ToUp25Seq1, Inverted::kDiagRightBankedUp25ToUp25Seq2, Inverted::kDiagRightBankedUp25ToUp25Seq3 }}, // DiagRightBankedUp25ToUp25
        {{ Inverted::kDiagDown25ToLeftBankedDown25Seq0, Inverted::kDiagDown25ToLeftBankedDown25Seq1, Inverted::kDiagDown25ToLeftBankedDown25Seq2, Inverted::kDiagDown25ToLeftBankedDown25Seq3 }}, // DiagDown25ToLeftBankedDown25
        {{ Inverted::kDiagDown25ToRightBankedDown25Seq0, Inverted::kDiagDown25ToRightBankedDown25Seq1, Inverted::kDiagDown25ToRightBankedDown25Seq2, Inverted::kDiagDown25ToRightBankedDown25Seq3 }}, // DiagDown25ToRightBankedDown25
        {{ Inverted::kDiagLeftBankedDown25ToDown25Seq0, Inverted::kDiagLeftBankedDown25ToDown25Seq1, Inverted::kDiagLeftBankedDown25ToDown25Seq2, Inverted::kDiagLeftBankedDown25ToDown25Seq3 }}, // DiagLeftBankedDown25ToDown25
        {{ Inverted::kDiagRightBankedDown25ToDown25Seq0, Inverted::kDiagRightBankedDown25ToDown25Seq1, Inverted::kDiagRightBankedDown25ToDown25Seq2, Inverted::kDiagRightBankedDown25ToDown25Seq3 }}, // DiagRightBankedDown25ToDown25
        {{ Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq0, Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq1, Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq2, Inverted::kDiagLeftBankedFlatToLeftBankedUp25Seq3 }}, // DiagLeftBankedFlatToLeftBankedUp25
        {{ Inverted::kDiagRightBankedFlatToRightBankedUp25Seq0, Inverted::kDiagRightBankedFlatToRightBankedUp25Seq1, Inverted::kDiagRightBankedFlatToRightBankedUp25Seq2, Inverted::kDiagRightBankedFlatToRightBankedUp25Seq3 }}, // DiagRightBankedFlatToRightBankedUp25
        {{ Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq0, Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq1, Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq2, Inverted::kDiagLeftBankedUp25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedUp25ToLeftBankedFlat
        {{ Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq0, Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq1, Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq2, Inverted::kDiagRightBankedUp25ToRightBankedFlatSeq3 }}, // DiagRightBankedUp25ToRightBankedFlat
        {{ Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq0, Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq1, Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq2, Inverted::kDiagLeftBankedFlatToLeftBankedDown25Seq3 }}, // DiagLeftBankedFlatToLeftBankedDown25
        {{ Inverted::kDiagRightBankedFlatToRightBankedDown25Seq0, Inverted::kDiagRightBankedFlatToRightBankedDown25Seq1, Inverted::kDiagRightBankedFlatToRightBankedDown25Seq2, Inverted::kDiagRightBankedFlatToRightBankedDown25Seq3 }}, // DiagRightBankedFlatToRightBankedDown25
        {{ Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq0, Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq1, Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq2, Inverted::kDiagLeftBankedDown25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedDown25ToLeftBankedFlat
        {{ Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq0, Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq1, Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq2, Inverted::kDiagRightBankedDown25ToRightBankedFlatSeq3 }}, // DiagRightBankedDown25ToRightBankedFlat
        {{ Inverted::kDiagFlatToLeftBankedUp25Seq0, Inverted::kDiagFlatToLeftBankedUp25Seq1, Inverted::kDiagFlatToLeftBankedUp25Seq2, Inverted::kDiagFlatToLeftBankedUp25Seq3 }}, // DiagFlatToLeftBankedUp25
        {{ Inverted::kDiagFlatToRightBankedUp25Seq0, Inverted::kDiagFlatToRightBankedUp25Seq1, Inverted::kDiagFlatToRightBankedUp25Seq2, Inverted::kDiagFlatToRightBankedUp25Seq3 }}, // DiagFlatToRightBankedUp25
        {{ Inverted::kDiagLeftBankedUp25ToFlatSeq0, Inverted::kDiagLeftBankedUp25ToFlatSeq1, Inverted::kDiagLeftBankedUp25ToFlatSeq2, Inverted::kDiagLeftBankedUp25ToFlatSeq3 }}, // DiagLeftBankedUp25ToFlat
        {{ Inverted::kDiagRightBankedUp25ToFlatSeq0, Inverted::kDiagRightBankedUp25ToFlatSeq1, Inverted::kDiagRightBankedUp25ToFlatSeq2, Inverted::kDiagRightBankedUp25ToFlatSeq3 }}, // DiagRightBankedUp25ToFlat
        {{ Inverted::kDiagFlatToLeftBankedDown25Seq0, Inverted::kDiagFlatToLeftBankedDown25Seq1, Inverted::kDiagFlatToLeftBankedDown25Seq2, Inverted::kDiagFlatToLeftBankedDown25Seq3 }}, // DiagFlatToLeftBankedDown25
        {{ Inverted::kDiagFlatToRightBankedDown25Seq0, Inverted::kDiagFlatToRightBankedDown25Seq1, Inverted::kDiagFlatToRightBankedDown25Seq2, Inverted::kDiagFlatToRightBankedDown25Seq3 }}, // DiagFlatToRightBankedDown25
        {{ Inverted::kDiagLeftBankedDown25ToFlatSeq0, Inverted::kDiagLeftBankedDown25ToFlatSeq1, Inverted::kDiagLeftBankedDown25ToFlatSeq2, Inverted::kDiagLeftBankedDown25ToFlatSeq3 }}, // DiagLeftBankedDown25ToFlat
        {{ Inverted::kDiagRightBankedDown25ToFlatSeq0, Inverted::kDiagRightBankedDown25ToFlatSeq1, Inverted::kDiagRightBankedDown25ToFlatSeq2, Inverted::kDiagRightBankedDown25ToFlatSeq3 }}, // DiagRightBankedDown25ToFlat
        {{ Inverted::kDiagUp25LeftBankedSeq0, Inverted::kDiagUp25LeftBankedSeq1, Inverted::kDiagUp25LeftBankedSeq2, Inverted::kDiagUp25LeftBankedSeq3 }}, // DiagUp25LeftBanked
        {{ Inverted::kDiagUp25RightBankedSeq0, Inverted::kDiagUp25RightBankedSeq1, Inverted::kDiagUp25RightBankedSeq2, Inverted::kDiagUp25RightBankedSeq3 }}, // DiagUp25RightBanked
        {{ Inverted::kDiagDown25LeftBankedSeq0, Inverted::kDiagDown25LeftBankedSeq1, Inverted::kDiagDown25LeftBankedSeq2, Inverted::kDiagDown25LeftBankedSeq3 }}, // DiagDown25LeftBanked
        {{ Inverted::kDiagDown25RightBankedSeq0, Inverted::kDiagDown25RightBankedSeq1, Inverted::kDiagDown25RightBankedSeq2, Inverted::kDiagDown25RightBankedSeq3 }}, // DiagDown25RightBanked
        {{ Inverted::kLeftEighthBankToDiagUp25Seq0, Inverted::kLeftEighthBankToDiagUp25Seq1, Inverted::kLeftEighthBankToDiagUp25Seq2, Inverted::kLeftEighthBankToDiagUp25Seq3, Inverted::kLeftEighthBankToDiagUp25Seq4 }}, // LeftEighthBankToDiagUp25
        {{ Inverted::kRightEighthBankToDiagUp25Seq0, Inverted::kRightEighthBankToDiagUp25Seq1, Inverted::kRightEighthBankToDiagUp25Seq2, Inverted::kRightEighthBankToDiagUp25Seq3, Inverted::kRightEighthBankToDiagUp25Seq4 }}, // RightEighthBankToDiagUp25
        {{ Inverted::kLeftEighthBankToDiagDown25Seq0, Inverted::kLeftEighthBankToDiagDown25Seq1, Inverted::kLeftEighthBankToDiagDown25Seq2, Inverted::kLeftEighthBankToDiagDown25Seq3, Inverted::kLeftEighthBankToDiagDown25Seq4 }}, // LeftEighthBankToDiagDown25
        {{ Inverted::kRightEighthBankToDiagDown25Seq0, Inverted::kRightEighthBankToDiagDown25Seq1, Inverted::kRightEighthBankToDiagDown25Seq2, Inverted::kRightEighthBankToDiagDown25Seq3, Inverted::kRightEighthBankToDiagDown25Seq4 }}, // RightEighthBankToDiagDown25
        {{ Inverted::kLeftEighthBankToOrthogonalUp25Seq0, Inverted::kLeftEighthBankToOrthogonalUp25Seq1, Inverted::kLeftEighthBankToOrthogonalUp25Seq2, Inverted::kLeftEighthBankToOrthogonalUp25Seq3, Inverted::kLeftEighthBankToOrthogonalUp25Seq4 }}, // LeftEighthBankToOrthogonalUp25
        {{ Inverted::kRightEighthBankToOrthogonalUp25Seq0, Inverted::kRightEighthBankToOrthogonalUp25Seq1, Inverted::kRightEighthBankToOrthogonalUp25Seq2, Inverted::kRightEighthBankToOrthogonalUp25Seq3, Inverted::kRightEighthBankToOrthogonalUp25Seq4 }}, // RightEighthBankToOrthogonalUp25
        {{ Inverted::kLeftEighthBankToOrthogonalDown25Seq0, Inverted::kLeftEighthBankToOrthogonalDown25Seq1, Inverted::kLeftEighthBankToOrthogonalDown25Seq2, Inverted::kLeftEighthBankToOrthogonalDown25Seq3, Inverted::kLeftEighthBankToOrthogonalDown25Seq4 }}, // LeftEighthBankToOrthogonalDown25
        {{ Inverted::kRightEighthBankToOrthogonalDown25Seq0, Inverted::kRightEighthBankToOrthogonalDown25Seq1, Inverted::kRightEighthBankToOrthogonalDown25Seq2, Inverted::kRightEighthBankToOrthogonalDown25Seq3, Inverted::kRightEighthBankToOrthogonalDown25Seq4 }}, // RightEighthBankToOrthogonalDown25
        {{ Inverted::kDiagBrakesSeq0, Inverted::kDiagBrakesSeq1, Inverted::kDiagBrakesSeq2, Inverted::kDiagBrakesSeq3 }}, // DiagBrakes
        {{ Inverted::kDiagBlockBrakesSeq0, Inverted::kDiagBlockBrakesSeq1, Inverted::kDiagBlockBrakesSeq2, Inverted::kDiagBlockBrakesSeq3 }}, // DiagBlockBrakes
        {{ Inverted::kDown25BrakesSeq0 }}, // Down25Brakes
        {{ Inverted::kDiagBoosterSeq0, Inverted::kDiagBoosterSeq1, Inverted::kDiagBoosterSeq2, Inverted::kDiagBoosterSeq3 }}, // DiagBooster
        {{ Inverted::kDiagFlatToUp60LongBaseSeq0, Inverted::kDiagFlatToUp60LongBaseSeq1, Inverted::kDiagFlatToUp60LongBaseSeq2, Inverted::kDiagFlatToUp60LongBaseSeq3, Inverted::kDiagFlatToUp60LongBaseSeq4, Inverted::kDiagFlatToUp60LongBaseSeq5, Inverted::kDiagFlatToUp60LongBaseSeq6, Inverted::kDiagFlatToUp60LongBaseSeq7, Inverted::kDiagFlatToUp60LongBaseSeq8, Inverted::kDiagFlatToUp60LongBaseSeq9 }}, // DiagFlatToUp60LongBase
        {{ Inverted::kDiagUp60ToFlatLongBaseSeq0, Inverted::kDiagUp60ToFlatLongBaseSeq1, Inverted::kDiagUp60ToFlatLongBaseSeq2, Inverted::kDiagUp60ToFlatLongBaseSeq3, Inverted::kDiagUp60ToFlatLongBaseSeq4, Inverted::kDiagUp60ToFlatLongBaseSeq5, Inverted::kDiagUp60ToFlatLongBaseSeq6, Inverted::kDiagUp60ToFlatLongBaseSeq7, Inverted::kDiagUp60ToFlatLongBaseSeq8, Inverted::kDiagUp60ToFlatLongBaseSeq9 }}, // DiagUp60ToFlatLongBase
        {{ Inverted::kDiagFlatToDown60LongBaseSeq0, Inverted::kDiagFlatToDown60LongBaseSeq1, Inverted::kDiagFlatToDown60LongBaseSeq2, Inverted::kDiagFlatToDown60LongBaseSeq3, Inverted::kDiagFlatToDown60LongBaseSeq4, Inverted::kDiagFlatToDown60LongBaseSeq5, Inverted::kDiagFlatToDown60LongBaseSeq6, Inverted::kDiagFlatToDown60LongBaseSeq7, Inverted::kDiagFlatToDown60LongBaseSeq8, Inverted::kDiagFlatToDown60LongBaseSeq9 }}, // DiagFlatToDown60LongBase
        {{ Inverted::kDiagDown60ToFlatLongBaseSeq0, Inverted::kDiagDown60ToFlatLongBaseSeq1, Inverted::kDiagDown60ToFlatLongBaseSeq2, Inverted::kDiagDown60ToFlatLongBaseSeq3, Inverted::kDiagDown60ToFlatLongBaseSeq4, Inverted::kDiagDown60ToFlatLongBaseSeq5, Inverted::kDiagDown60ToFlatLongBaseSeq6, Inverted::kDiagDown60ToFlatLongBaseSeq7, Inverted::kDiagDown60ToFlatLongBaseSeq8, Inverted::kDiagDown60ToFlatLongBaseSeq9 }}, // DiagDown60ToFlatLongBase
        {{ Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq0, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq1, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq2, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq3, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq4, Inverted::kLeftEighthDiveLoopUpToOrthogonalSeq5 }}, // LeftEighthDiveLoopUpToOrthogonal
        {{ Inverted::kRightEighthDiveLoopUpToOrthogonalSeq0, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq1, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq2, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq3, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq4, Inverted::kRightEighthDiveLoopUpToOrthogonalSeq5 }}, // RightEighthDiveLoopUpToOrthogonal
        {{ Inverted::kLeftEighthDiveLoopDownToDiagSeq0, Inverted::kLeftEighthDiveLoopDownToDiagSeq1, Inverted::kLeftEighthDiveLoopDownToDiagSeq2, Inverted::kLeftEighthDiveLoopDownToDiagSeq3, Inverted::kLeftEighthDiveLoopDownToDiagSeq4, Inverted::kLeftEighthDiveLoopDownToDiagSeq5 }}, // LeftEighthDiveLoopDownToDiag
        {{ Inverted::kRightEighthDiveLoopDownToDiagSeq0, Inverted::kRightEighthDiveLoopDownToDiagSeq1, Inverted::kRightEighthDiveLoopDownToDiagSeq2, Inverted::kRightEighthDiveLoopDownToDiagSeq3, Inverted::kRightEighthDiveLoopDownToDiagSeq4, Inverted::kRightEighthDiveLoopDownToDiagSeq5 }}, // RightEighthDiveLoopDownToDiag
        {{ Inverted::kDiagDown25BrakesSeq0, Inverted::kDiagDown25BrakesSeq1, Inverted::kDiagDown25BrakesSeq2, Inverted::kDiagDown25BrakesSeq3 }}, // DiagDown25Brakes
    };

    static_assert(std::size(kBlockedSegmentsSuspendedSwingingTrain) == EnumValue(TrackElemType::Count));

    // kBlockedSegmentsWideTrain is based on the uninverted Multi Dimension Roller Coaster.
    // It is not properly filled out for pieces that it doesn't have.
    // It is a mix of wide and narrow blocks.
    static constexpr const std::array<uint16_t, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> kBlockedSegmentsWideTrain[] = {
        {{ Wide::kFlatSeq0 }}, // Flat
        {{ Wide::kEndStationSeq0 }}, // EndStation
        {{ Wide::kBeginStationSeq0 }}, // BeginStation
        {{ Wide::kMiddleStationSeq0 }}, // MiddleStation
        {{ Wide::kUp25Seq0 }}, // Up25
        {{ Wide::kUp60Seq0 }}, // Up60
        {{ Wide::kFlatToUp25Seq0 }}, // FlatToUp25
        {{ Wide::kUp25ToUp60Seq0 }}, // Up25ToUp60
        {{ Wide::kUp60ToUp25Seq0 }}, // Up60ToUp25
        {{ Wide::kUp25ToFlatSeq0 }}, // Up25ToFlat
        {{ Wide::kDown25Seq0 }}, // Down25
        {{ Wide::kDown60Seq0 }}, // Down60
        {{ Wide::kFlatToDown25Seq0 }}, // FlatToDown25
        {{ Wide::kDown25ToDown60Seq0 }}, // Down25ToDown60
        {{ Wide::kDown60ToDown25Seq0 }}, // Down60ToDown25
        {{ Wide::kDown25ToFlatSeq0 }}, // Down25ToFlat
        {{ Wide::kLeftQuarterTurn5TilesSeq0, Narrow::kLeftQuarterTurn5TilesSeq1, Narrow::kLeftQuarterTurn5TilesSeq2, Narrow::kLeftQuarterTurn5TilesSeq3, Narrow::kLeftQuarterTurn5TilesSeq4, Narrow::kLeftQuarterTurn5TilesSeq5, Narrow::kLeftQuarterTurn5TilesSeq6 }}, // LeftQuarterTurn5Tiles
        {{ Narrow::kRightQuarterTurn5TilesSeq0, Narrow::kRightQuarterTurn5TilesSeq1, Narrow::kRightQuarterTurn5TilesSeq2, Narrow::kRightQuarterTurn5TilesSeq3, Narrow::kRightQuarterTurn5TilesSeq4, Narrow::kRightQuarterTurn5TilesSeq5, Wide::kRightQuarterTurn5TilesSeq6 }}, // RightQuarterTurn5Tiles
        {{ Wide::kFlatToLeftBankSeq0 }}, // FlatToLeftBank
        {{ Wide::kFlatToRightBankSeq0 }}, // FlatToRightBank
        {{ Wide::kLeftBankToFlatSeq0 }}, // LeftBankToFlat
        {{ Wide::kRightBankToFlatSeq0 }}, // RightBankToFlat
        {{ Wide::kBankedLeftQuarterTurn5TilesSeq0, Narrow::kBankedLeftQuarterTurn5TilesSeq1, Narrow::kBankedLeftQuarterTurn5TilesSeq2, Narrow::kBankedLeftQuarterTurn5TilesSeq3, Narrow::kBankedLeftQuarterTurn5TilesSeq4, Narrow::kBankedLeftQuarterTurn5TilesSeq5, Narrow::kBankedLeftQuarterTurn5TilesSeq6 }}, // BankedLeftQuarterTurn5Tiles
        {{ Narrow::kBankedRightQuarterTurn5TilesSeq0, Narrow::kBankedRightQuarterTurn5TilesSeq1, Narrow::kBankedRightQuarterTurn5TilesSeq2, Narrow::kBankedRightQuarterTurn5TilesSeq3, Narrow::kBankedRightQuarterTurn5TilesSeq4, Narrow::kBankedRightQuarterTurn5TilesSeq5, Wide::kBankedRightQuarterTurn5TilesSeq6 }}, // BankedRightQuarterTurn5Tiles
        {{ Wide::kLeftBankToUp25Seq0 }}, // LeftBankToUp25
        {{ Wide::kRightBankToUp25Seq0 }}, // RightBankToUp25
        {{ Wide::kUp25ToLeftBankSeq0 }}, // Up25ToLeftBank
        {{ Wide::kUp25ToRightBankSeq0 }}, // Up25ToRightBank
        {{ Wide::kLeftBankToDown25Seq0 }}, // LeftBankToDown25
        {{ Wide::kRightBankToDown25Seq0 }}, // RightBankToDown25
        {{ Wide::kDown25ToLeftBankSeq0 }}, // Down25ToLeftBank
        {{ Wide::kDown25ToRightBankSeq0 }}, // Down25ToRightBank
        {{ Wide::kLeftBankSeq0 }}, // LeftBank
        {{ Wide::kRightBankSeq0 }}, // RightBank
        {{ Narrow::kLeftQuarterTurn5TilesUp25Seq0, Narrow::kLeftQuarterTurn5TilesUp25Seq1, Narrow::kLeftQuarterTurn5TilesUp25Seq2, Narrow::kLeftQuarterTurn5TilesUp25Seq3, Narrow::kLeftQuarterTurn5TilesUp25Seq4, Narrow::kLeftQuarterTurn5TilesUp25Seq5, Narrow::kLeftQuarterTurn5TilesUp25Seq6 }}, // LeftQuarterTurn5TilesUp25
        {{ Narrow::kRightQuarterTurn5TilesUp25Seq0, Narrow::kRightQuarterTurn5TilesUp25Seq1, Narrow::kRightQuarterTurn5TilesUp25Seq2, Narrow::kRightQuarterTurn5TilesUp25Seq3, Narrow::kRightQuarterTurn5TilesUp25Seq4, Narrow::kRightQuarterTurn5TilesUp25Seq5, Narrow::kRightQuarterTurn5TilesUp25Seq6 }}, // RightQuarterTurn5TilesUp25
        {{ Narrow::kLeftQuarterTurn5TilesDown25Seq0, Narrow::kLeftQuarterTurn5TilesDown25Seq1, Narrow::kLeftQuarterTurn5TilesDown25Seq2, Narrow::kLeftQuarterTurn5TilesDown25Seq3, Narrow::kLeftQuarterTurn5TilesDown25Seq4, Narrow::kLeftQuarterTurn5TilesDown25Seq5, Narrow::kLeftQuarterTurn5TilesDown25Seq6 }}, // LeftQuarterTurn5TilesDown25
        {{ Narrow::kRightQuarterTurn5TilesDown25Seq0, Narrow::kRightQuarterTurn5TilesDown25Seq1, Narrow::kRightQuarterTurn5TilesDown25Seq2, Narrow::kRightQuarterTurn5TilesDown25Seq3, Narrow::kRightQuarterTurn5TilesDown25Seq4, Narrow::kRightQuarterTurn5TilesDown25Seq5, Narrow::kRightQuarterTurn5TilesDown25Seq6 }}, // RightQuarterTurn5TilesDown25
        {{ Wide::kSBendLeftSeq0, WideTrain::kSBendLeftSeq1, WideTrain::kSBendLeftSeq2, Wide::kSBendLeftSeq3 }}, // SBendLeft
        {{ Wide::kSBendRightSeq0, WideTrain::kSBendRightSeq1, WideTrain::kSBendRightSeq2, Wide::kSBendRightSeq3 }}, // SBendRight
        {{ Narrow::kLeftVerticalLoopSeq0, Narrow::kLeftVerticalLoopSeq1, Narrow::kLeftVerticalLoopSeq2, Narrow::kLeftVerticalLoopSeq3, Narrow::kLeftVerticalLoopSeq4, Narrow::kLeftVerticalLoopSeq5, Narrow::kLeftVerticalLoopSeq6, Narrow::kLeftVerticalLoopSeq7, Narrow::kLeftVerticalLoopSeq8, Narrow::kLeftVerticalLoopSeq9 }}, // LeftVerticalLoop
        {{ Narrow::kRightVerticalLoopSeq0, Narrow::kRightVerticalLoopSeq1, Narrow::kRightVerticalLoopSeq2, Narrow::kRightVerticalLoopSeq3, Narrow::kRightVerticalLoopSeq4, Narrow::kRightVerticalLoopSeq5, Narrow::kRightVerticalLoopSeq6, Narrow::kRightVerticalLoopSeq7, Narrow::kRightVerticalLoopSeq8, Narrow::kRightVerticalLoopSeq9 }}, // RightVerticalLoop
        {{ Wide::kLeftQuarterTurn3TilesSeq0, Narrow::kLeftQuarterTurn3TilesSeq1, Narrow::kLeftQuarterTurn3TilesSeq2, Narrow::kLeftQuarterTurn3TilesSeq3 }}, // LeftQuarterTurn3Tiles
        {{ Narrow::kRightQuarterTurn3TilesSeq0, Narrow::kRightQuarterTurn3TilesSeq1, Narrow::kRightQuarterTurn3TilesSeq2, Wide::kRightQuarterTurn3TilesSeq3 }}, // RightQuarterTurn3Tiles
        {{ Wide::kLeftBankedQuarterTurn3TilesSeq0, Narrow::kLeftBankedQuarterTurn3TilesSeq1, Narrow::kLeftBankedQuarterTurn3TilesSeq2, Narrow::kLeftBankedQuarterTurn3TilesSeq3 }}, // LeftBankedQuarterTurn3Tiles
        {{ Narrow::kRightBankedQuarterTurn3TilesSeq0, Narrow::kRightBankedQuarterTurn3TilesSeq1, Narrow::kRightBankedQuarterTurn3TilesSeq2, Wide::kRightBankedQuarterTurn3TilesSeq3 }}, // RightBankedQuarterTurn3Tiles
        {{ Narrow::kLeftQuarterTurn3TilesUp25Seq0, Narrow::kLeftQuarterTurn3TilesUp25Seq1, Narrow::kLeftQuarterTurn3TilesUp25Seq2, Narrow::kLeftQuarterTurn3TilesUp25Seq3 }}, // LeftQuarterTurn3TilesUp25
        {{ Narrow::kRightQuarterTurn3TilesUp25Seq0, Narrow::kRightQuarterTurn3TilesUp25Seq1, Narrow::kRightQuarterTurn3TilesUp25Seq2, Narrow::kRightQuarterTurn3TilesUp25Seq3 }}, // RightQuarterTurn3TilesUp25
        {{ Narrow::kLeftQuarterTurn3TilesDown25Seq0, Narrow::kLeftQuarterTurn3TilesDown25Seq1, Narrow::kLeftQuarterTurn3TilesDown25Seq2, Narrow::kLeftQuarterTurn3TilesDown25Seq3 }}, // LeftQuarterTurn3TilesDown25
        {{ Narrow::kRightQuarterTurn3TilesDown25Seq0, Narrow::kRightQuarterTurn3TilesDown25Seq1, Narrow::kRightQuarterTurn3TilesDown25Seq2, Narrow::kRightQuarterTurn3TilesDown25Seq3 }}, // RightQuarterTurn3TilesDown25
        {{ Wide::kLeftQuarterTurn1TileSeq0 }}, // LeftQuarterTurn1Tile
        {{ Wide::kRightQuarterTurn1TileSeq0 }}, // RightQuarterTurn1Tile
        {{ Narrow::kLeftTwistDownToUpSeq0, Narrow::kLeftTwistDownToUpSeq1, Narrow::kLeftTwistDownToUpSeq2 }}, // LeftTwistDownToUp
        {{ Narrow::kRightTwistDownToUpSeq0, Narrow::kRightTwistDownToUpSeq1, Narrow::kRightTwistDownToUpSeq2 }}, // RightTwistDownToUp
        {{ Narrow::kLeftTwistUpToDownSeq0, Narrow::kLeftTwistUpToDownSeq1, Narrow::kLeftTwistUpToDownSeq2 }}, // LeftTwistUpToDown
        {{ Narrow::kRightTwistUpToDownSeq0, Narrow::kRightTwistUpToDownSeq1, Narrow::kRightTwistUpToDownSeq2 }}, // RightTwistUpToDown
        {{ Narrow::kHalfLoopUpSeq0, Narrow::kHalfLoopUpSeq1, Narrow::kHalfLoopUpSeq2, Narrow::kHalfLoopUpSeq3 }}, // HalfLoopUp
        {{ Narrow::kHalfLoopDownSeq0, Narrow::kHalfLoopDownSeq1, Narrow::kHalfLoopDownSeq2, Narrow::kHalfLoopDownSeq3 }}, // HalfLoopDown
        {{ Narrow::kLeftCorkscrewUpSeq0, Narrow::kLeftCorkscrewUpSeq1, Narrow::kLeftCorkscrewUpSeq2 }}, // LeftCorkscrewUp
        {{ Narrow::kRightCorkscrewUpSeq0, Narrow::kRightCorkscrewUpSeq1, Narrow::kRightCorkscrewUpSeq2 }}, // RightCorkscrewUp
        {{ Narrow::kLeftCorkscrewDownSeq0, Narrow::kLeftCorkscrewDownSeq1, Narrow::kLeftCorkscrewDownSeq2 }}, // LeftCorkscrewDown
        {{ Narrow::kRightCorkscrewDownSeq0, Narrow::kRightCorkscrewDownSeq1, Narrow::kRightCorkscrewDownSeq2 }}, // RightCorkscrewDown
        {{ Wide::kFlatToUp60Seq0 }}, // FlatToUp60
        {{ Wide::kUp60ToFlatSeq0 }}, // Up60ToFlat
        {{ Wide::kFlatToDown60Seq0 }}, // FlatToDown60
        {{ Wide::kDown60ToFlatSeq0 }}, // Down60ToFlat
        {{ Narrow::kTowerBaseSeq0, Narrow::kTowerBaseSeq1, Narrow::kTowerBaseSeq2, Narrow::kTowerBaseSeq3, Narrow::kTowerBaseSeq4, Narrow::kTowerBaseSeq5, Narrow::kTowerBaseSeq6, Narrow::kTowerBaseSeq7, Narrow::kTowerBaseSeq8 }}, // TowerBase
        {{ Narrow::kTowerSectionSeq0, Narrow::kTowerSectionSeq1 }}, // TowerSection
        {{ Wide::kFlatCoveredSeq0 }}, // FlatCovered
        {{ Wide::kUp25CoveredSeq0 }}, // Up25Covered
        {{ Wide::kUp60CoveredSeq0 }}, // Up60Covered
        {{ Wide::kFlatToUp25CoveredSeq0 }}, // FlatToUp25Covered
        {{ Wide::kUp25ToUp60CoveredSeq0 }}, // Up25ToUp60Covered
        {{ Wide::kUp60ToUp25CoveredSeq0 }}, // Up60ToUp25Covered
        {{ Wide::kUp25ToFlatCoveredSeq0 }}, // Up25ToFlatCovered
        {{ Wide::kDown25CoveredSeq0 }}, // Down25Covered
        {{ Wide::kDown60CoveredSeq0 }}, // Down60Covered
        {{ Wide::kFlatToDown25CoveredSeq0 }}, // FlatToDown25Covered
        {{ Wide::kDown25ToDown60CoveredSeq0 }}, // Down25ToDown60Covered
        {{ Wide::kDown60ToDown25CoveredSeq0 }}, // Down60ToDown25Covered
        {{ Wide::kDown25ToFlatCoveredSeq0 }}, // Down25ToFlatCovered
        {{ Wide::kLeftQuarterTurn5TilesCoveredSeq0, Narrow::kLeftQuarterTurn5TilesCoveredSeq1, Narrow::kLeftQuarterTurn5TilesCoveredSeq2, Narrow::kLeftQuarterTurn5TilesCoveredSeq3, Narrow::kLeftQuarterTurn5TilesCoveredSeq4, Narrow::kLeftQuarterTurn5TilesCoveredSeq5, Narrow::kLeftQuarterTurn5TilesCoveredSeq6 }}, // LeftQuarterTurn5TilesCovered
        {{ Narrow::kRightQuarterTurn5TilesCoveredSeq0, Narrow::kRightQuarterTurn5TilesCoveredSeq1, Narrow::kRightQuarterTurn5TilesCoveredSeq2, Narrow::kRightQuarterTurn5TilesCoveredSeq3, Narrow::kRightQuarterTurn5TilesCoveredSeq4, Narrow::kRightQuarterTurn5TilesCoveredSeq5, Wide::kRightQuarterTurn5TilesCoveredSeq6 }}, // RightQuarterTurn5TilesCovered
        {{ Wide::kSBendLeftCoveredSeq0, WideTrain::kSBendLeftCoveredSeq1, WideTrain::kSBendLeftCoveredSeq2, Wide::kSBendLeftCoveredSeq3 }}, // SBendLeftCovered
        {{ Wide::kSBendRightCoveredSeq0, WideTrain::kSBendRightCoveredSeq1, WideTrain::kSBendRightCoveredSeq2, Wide::kSBendRightCoveredSeq3 }}, // SBendRightCovered
        {{ Wide::kLeftQuarterTurn3TilesCoveredSeq0, Narrow::kLeftQuarterTurn3TilesCoveredSeq1, Narrow::kLeftQuarterTurn3TilesCoveredSeq2, Narrow::kLeftQuarterTurn3TilesCoveredSeq3 }}, // LeftQuarterTurn3TilesCovered
        {{ Narrow::kRightQuarterTurn3TilesCoveredSeq0, Narrow::kRightQuarterTurn3TilesCoveredSeq1, Narrow::kRightQuarterTurn3TilesCoveredSeq2, Wide::kRightQuarterTurn3TilesCoveredSeq3 }}, // RightQuarterTurn3TilesCovered
        {{ Wide::kLeftHalfBankedHelixUpSmallSeq0, Narrow::kLeftHalfBankedHelixUpSmallSeq1, Narrow::kLeftHalfBankedHelixUpSmallSeq2, Narrow::kLeftHalfBankedHelixUpSmallSeq3, Wide::kLeftHalfBankedHelixUpSmallSeq4, Narrow::kLeftHalfBankedHelixUpSmallSeq5, Narrow::kLeftHalfBankedHelixUpSmallSeq6, Wide::kLeftHalfBankedHelixUpSmallSeq7 }}, // LeftHalfBankedHelixUpSmall
        {{ Wide::kRightHalfBankedHelixUpSmallSeq0, Narrow::kRightHalfBankedHelixUpSmallSeq1, Narrow::kRightHalfBankedHelixUpSmallSeq2, Wide::kRightHalfBankedHelixUpSmallSeq3, Narrow::kRightHalfBankedHelixUpSmallSeq4, Narrow::kRightHalfBankedHelixUpSmallSeq5, Narrow::kRightHalfBankedHelixUpSmallSeq6, Wide::kRightHalfBankedHelixUpSmallSeq7 }}, // RightHalfBankedHelixUpSmall
        {{ Wide::kLeftHalfBankedHelixDownSmallSeq0, Narrow::kLeftHalfBankedHelixDownSmallSeq1, Narrow::kLeftHalfBankedHelixDownSmallSeq2, Wide::kLeftHalfBankedHelixDownSmallSeq3, Wide::kLeftHalfBankedHelixDownSmallSeq4, Narrow::kLeftHalfBankedHelixDownSmallSeq5, Narrow::kLeftHalfBankedHelixDownSmallSeq6, Wide::kLeftHalfBankedHelixDownSmallSeq7 }}, // LeftHalfBankedHelixDownSmall
        {{ Narrow::kRightHalfBankedHelixDownSmallSeq0, Narrow::kRightHalfBankedHelixDownSmallSeq1, Narrow::kRightHalfBankedHelixDownSmallSeq2, Wide::kRightHalfBankedHelixDownSmallSeq3, Narrow::kRightHalfBankedHelixDownSmallSeq4, Narrow::kRightHalfBankedHelixDownSmallSeq5, Narrow::kRightHalfBankedHelixDownSmallSeq6, Wide::kRightHalfBankedHelixDownSmallSeq7 }}, // RightHalfBankedHelixDownSmall
        {{ Wide::kLeftHalfBankedHelixUpLargeSeq0, Narrow::kLeftHalfBankedHelixUpLargeSeq1, Narrow::kLeftHalfBankedHelixUpLargeSeq2, Narrow::kLeftHalfBankedHelixUpLargeSeq3, Narrow::kLeftHalfBankedHelixUpLargeSeq4, Narrow::kLeftHalfBankedHelixUpLargeSeq5, Narrow::kLeftHalfBankedHelixUpLargeSeq6, Wide::kLeftHalfBankedHelixUpLargeSeq7, Narrow::kLeftHalfBankedHelixUpLargeSeq8, Narrow::kLeftHalfBankedHelixUpLargeSeq9, Narrow::kLeftHalfBankedHelixUpLargeSeq10, Narrow::kLeftHalfBankedHelixUpLargeSeq11, Narrow::kLeftHalfBankedHelixUpLargeSeq12, Wide::kLeftHalfBankedHelixUpLargeSeq13 }}, // LeftHalfBankedHelixUpLarge
        {{ Wide::kRightHalfBankedHelixUpLargeSeq0, Narrow::kRightHalfBankedHelixUpLargeSeq1, Narrow::kRightHalfBankedHelixUpLargeSeq2, Narrow::kRightHalfBankedHelixUpLargeSeq3, Narrow::kRightHalfBankedHelixUpLargeSeq4, Narrow::kRightHalfBankedHelixUpLargeSeq5, Wide::kRightHalfBankedHelixUpLargeSeq6, Narrow::kRightHalfBankedHelixUpLargeSeq7, Narrow::kRightHalfBankedHelixUpLargeSeq8, Narrow::kRightHalfBankedHelixUpLargeSeq9, Narrow::kRightHalfBankedHelixUpLargeSeq10, Narrow::kRightHalfBankedHelixUpLargeSeq11, Narrow::kRightHalfBankedHelixUpLargeSeq12, Wide::kRightHalfBankedHelixUpLargeSeq13 }}, // RightHalfBankedHelixUpLarge
        {{ Wide::kLeftHalfBankedHelixDownLargeSeq0, Narrow::kLeftHalfBankedHelixDownLargeSeq1, Narrow::kLeftHalfBankedHelixDownLargeSeq2, Narrow::kLeftHalfBankedHelixDownLargeSeq3, Narrow::kLeftHalfBankedHelixDownLargeSeq4, Narrow::kLeftHalfBankedHelixDownLargeSeq5, Wide::kLeftHalfBankedHelixDownLargeSeq6, Wide::kLeftHalfBankedHelixDownLargeSeq7, Narrow::kLeftHalfBankedHelixDownLargeSeq8, Narrow::kLeftHalfBankedHelixDownLargeSeq9, Narrow::kLeftHalfBankedHelixDownLargeSeq10, Narrow::kLeftHalfBankedHelixDownLargeSeq11, Narrow::kLeftHalfBankedHelixDownLargeSeq12, Wide::kLeftHalfBankedHelixDownLargeSeq13 }}, // LeftHalfBankedHelixDownLarge
        {{ Narrow::kRightHalfBankedHelixDownLargeSeq0, Narrow::kRightHalfBankedHelixDownLargeSeq1, Narrow::kRightHalfBankedHelixDownLargeSeq2, Narrow::kRightHalfBankedHelixDownLargeSeq3, Narrow::kRightHalfBankedHelixDownLargeSeq4, Narrow::kRightHalfBankedHelixDownLargeSeq5, Wide::kRightHalfBankedHelixDownLargeSeq6, Narrow::kRightHalfBankedHelixDownLargeSeq7, Narrow::kRightHalfBankedHelixDownLargeSeq8, Narrow::kRightHalfBankedHelixDownLargeSeq9, Narrow::kRightHalfBankedHelixDownLargeSeq10, Narrow::kRightHalfBankedHelixDownLargeSeq11, Narrow::kRightHalfBankedHelixDownLargeSeq12, Wide::kRightHalfBankedHelixDownLargeSeq13 }}, // RightHalfBankedHelixDownLarge
        {{ Wide::kLeftQuarterTurn1TileUp60Seq0 }}, // LeftQuarterTurn1TileUp60
        {{ Wide::kRightQuarterTurn1TileUp60Seq0 }}, // RightQuarterTurn1TileUp60
        {{ Wide::kLeftQuarterTurn1TileDown60Seq0 }}, // LeftQuarterTurn1TileDown60
        {{ Wide::kRightQuarterTurn1TileDown60Seq0 }}, // RightQuarterTurn1TileDown60
        {{ Wide::kBrakesSeq0 }}, // Brakes
        {{ Wide::kBoosterSeq0 }}, // Booster
        {{ Wide::kMazeSeq0 }}, // Maze
        {{ Narrow::kLeftQuarterBankedHelixLargeUpSeq0, Narrow::kLeftQuarterBankedHelixLargeUpSeq1, Narrow::kLeftQuarterBankedHelixLargeUpSeq2, Narrow::kLeftQuarterBankedHelixLargeUpSeq3, Narrow::kLeftQuarterBankedHelixLargeUpSeq4, Narrow::kLeftQuarterBankedHelixLargeUpSeq5, Narrow::kLeftQuarterBankedHelixLargeUpSeq6 }}, // LeftQuarterBankedHelixLargeUp
        {{ Narrow::kRightQuarterBankedHelixLargeUpSeq0, Narrow::kRightQuarterBankedHelixLargeUpSeq1, Narrow::kRightQuarterBankedHelixLargeUpSeq2, Narrow::kRightQuarterBankedHelixLargeUpSeq3, Narrow::kRightQuarterBankedHelixLargeUpSeq4, Narrow::kRightQuarterBankedHelixLargeUpSeq5, Narrow::kRightQuarterBankedHelixLargeUpSeq6 }}, // RightQuarterBankedHelixLargeUp
        {{ Narrow::kLeftQuarterBankedHelixLargeDownSeq0, Narrow::kLeftQuarterBankedHelixLargeDownSeq1, Narrow::kLeftQuarterBankedHelixLargeDownSeq2, Narrow::kLeftQuarterBankedHelixLargeDownSeq3, Narrow::kLeftQuarterBankedHelixLargeDownSeq4, Narrow::kLeftQuarterBankedHelixLargeDownSeq5, Narrow::kLeftQuarterBankedHelixLargeDownSeq6 }}, // LeftQuarterBankedHelixLargeDown
        {{ Narrow::kRightQuarterBankedHelixLargeDownSeq0, Narrow::kRightQuarterBankedHelixLargeDownSeq1, Narrow::kRightQuarterBankedHelixLargeDownSeq2, Narrow::kRightQuarterBankedHelixLargeDownSeq3, Narrow::kRightQuarterBankedHelixLargeDownSeq4, Narrow::kRightQuarterBankedHelixLargeDownSeq5, Narrow::kRightQuarterBankedHelixLargeDownSeq6 }}, // RightQuarterBankedHelixLargeDown
        {{ Narrow::kLeftQuarterHelixLargeUpSeq0, Narrow::kLeftQuarterHelixLargeUpSeq1, Narrow::kLeftQuarterHelixLargeUpSeq2, Narrow::kLeftQuarterHelixLargeUpSeq3, Narrow::kLeftQuarterHelixLargeUpSeq4, Narrow::kLeftQuarterHelixLargeUpSeq5, Narrow::kLeftQuarterHelixLargeUpSeq6 }}, // LeftQuarterHelixLargeUp
        {{ Narrow::kRightQuarterHelixLargeUpSeq0, Narrow::kRightQuarterHelixLargeUpSeq1, Narrow::kRightQuarterHelixLargeUpSeq2, Narrow::kRightQuarterHelixLargeUpSeq3, Narrow::kRightQuarterHelixLargeUpSeq4, Narrow::kRightQuarterHelixLargeUpSeq5, Narrow::kRightQuarterHelixLargeUpSeq6 }}, // RightQuarterHelixLargeUp
        {{ Narrow::kLeftQuarterHelixLargeDownSeq0, Narrow::kLeftQuarterHelixLargeDownSeq1, Narrow::kLeftQuarterHelixLargeDownSeq2, Narrow::kLeftQuarterHelixLargeDownSeq3, Narrow::kLeftQuarterHelixLargeDownSeq4, Narrow::kLeftQuarterHelixLargeDownSeq5, Narrow::kLeftQuarterHelixLargeDownSeq6 }}, // LeftQuarterHelixLargeDown
        {{ Narrow::kRightQuarterHelixLargeDownSeq0, Narrow::kRightQuarterHelixLargeDownSeq1, Narrow::kRightQuarterHelixLargeDownSeq2, Narrow::kRightQuarterHelixLargeDownSeq3, Narrow::kRightQuarterHelixLargeDownSeq4, Narrow::kRightQuarterHelixLargeDownSeq5, Narrow::kRightQuarterHelixLargeDownSeq6 }}, // RightQuarterHelixLargeDown
        {{ Wide::kUp25LeftBankedSeq0 }}, // Up25LeftBanked
        {{ Wide::kUp25RightBankedSeq0 }}, // Up25RightBanked
        {{ Wide::kWaterfallSeq0 }}, // Waterfall
        {{ Wide::kRapidsSeq0 }}, // Rapids
        {{ Wide::kOnRidePhotoSeq0 }}, // OnRidePhoto
        {{ Wide::kDown25LeftBankedSeq0 }}, // Down25LeftBanked
        {{ Wide::kDown25RightBankedSeq0 }}, // Down25RightBanked
        {{ Wide::kWatersplashSeq0, Wide::kWatersplashSeq1, Wide::kWatersplashSeq2, Wide::kWatersplashSeq3, Wide::kWatersplashSeq4 }}, // Watersplash
        {{ Wide::kFlatToUp60LongBaseSeq0, Wide::kFlatToUp60LongBaseSeq1, Wide::kFlatToUp60LongBaseSeq2, Wide::kFlatToUp60LongBaseSeq3 }}, // FlatToUp60LongBase
        {{ Wide::kUp60ToFlatLongBaseSeq0, Wide::kUp60ToFlatLongBaseSeq1, Wide::kUp60ToFlatLongBaseSeq2, Wide::kUp60ToFlatLongBaseSeq3 }}, // Up60ToFlatLongBase
        {{ Wide::kWhirlpoolSeq0 }}, // Whirlpool
        {{ Wide::kDown60ToFlatLongBaseSeq0, Wide::kDown60ToFlatLongBaseSeq1, Wide::kDown60ToFlatLongBaseSeq2, Wide::kDown60ToFlatLongBaseSeq3 }}, // Down60ToFlatLongBase
        {{ Wide::kFlatToDown60LongBaseSeq0, Wide::kFlatToDown60LongBaseSeq1, Wide::kFlatToDown60LongBaseSeq2, Wide::kFlatToDown60LongBaseSeq3 }}, // FlatToDown60LongBase
        {{ Wide::kCableLiftHillSeq0, Wide::kCableLiftHillSeq1, Wide::kCableLiftHillSeq2, Wide::kCableLiftHillSeq3 }}, // CableLiftHill
        {{ Wide::kReverseFreefallSlopeSeq0, Wide::kReverseFreefallSlopeSeq1, Wide::kReverseFreefallSlopeSeq2, Wide::kReverseFreefallSlopeSeq3, Wide::kReverseFreefallSlopeSeq4, Wide::kReverseFreefallSlopeSeq5, Wide::kReverseFreefallSlopeSeq6 }}, // ReverseFreefallSlope
        {{ Wide::kReverseFreefallVerticalSeq0, Wide::kReverseFreefallVerticalSeq1 }}, // ReverseFreefallVertical
        {{ Wide::kUp90Seq0, Wide::kUp90Seq1 }}, // Up90
        {{ Wide::kDown90Seq0, Wide::kDown90Seq1 }}, // Down90
        {{ Wide::kUp60ToUp90Seq0, Wide::kUp60ToUp90Seq1 }}, // Up60ToUp90
        {{ Wide::kDown90ToDown60Seq0 }}, // Down90ToDown60
        {{ Wide::kUp90ToUp60Seq0 }}, // Up90ToUp60
        {{ Wide::kDown60ToDown90Seq0, Wide::kDown60ToDown90Seq1 }}, // Down60ToDown90
        {{ Wide::kBrakeForDropSeq0 }}, // BrakeForDrop
        {{ Wide::kLeftEighthToDiagSeq0, Wide::kLeftEighthToDiagSeq1, Narrow::kLeftEighthToDiagSeq2, Narrow::kLeftEighthToDiagSeq3, Wide::kLeftEighthToDiagSeq4 }}, // LeftEighthToDiag
        {{ Wide::kRightEighthToDiagSeq0, Wide::kRightEighthToDiagSeq1, Narrow::kRightEighthToDiagSeq2, Narrow::kRightEighthToDiagSeq3, Wide::kRightEighthToDiagSeq4 }}, // RightEighthToDiag
        {{ Wide::kLeftEighthToOrthogonalSeq0, Narrow::kLeftEighthToOrthogonalSeq1, Narrow::kLeftEighthToOrthogonalSeq2, Wide::kLeftEighthToOrthogonalSeq3, Wide::kLeftEighthToOrthogonalSeq4 }}, // LeftEighthToOrthogonal
        {{ Wide::kRightEighthToOrthogonalSeq0, Narrow::kRightEighthToOrthogonalSeq1, Narrow::kRightEighthToOrthogonalSeq2, Wide::kRightEighthToOrthogonalSeq3, Wide::kRightEighthToOrthogonalSeq4 }}, // RightEighthToOrthogonal
        {{ Wide::kLeftEighthBankToDiagSeq0, Wide::kLeftEighthBankToDiagSeq1, Narrow::kLeftEighthBankToDiagSeq2, Narrow::kLeftEighthBankToDiagSeq3, Wide::kLeftEighthBankToDiagSeq4 }}, // LeftEighthBankToDiag
        {{ Wide::kRightEighthBankToDiagSeq0, Wide::kRightEighthBankToDiagSeq1, Narrow::kRightEighthBankToDiagSeq2, Narrow::kRightEighthBankToDiagSeq3, Wide::kRightEighthBankToDiagSeq4 }}, // RightEighthBankToDiag
        {{ Wide::kLeftEighthBankToOrthogonalSeq0, Narrow::kLeftEighthBankToOrthogonalSeq1, Narrow::kLeftEighthBankToOrthogonalSeq2, Wide::kLeftEighthBankToOrthogonalSeq3, Wide::kLeftEighthBankToOrthogonalSeq4 }}, // LeftEighthBankToOrthogonal
        {{ Wide::kRightEighthBankToOrthogonalSeq0, Narrow::kRightEighthBankToOrthogonalSeq1, Narrow::kRightEighthBankToOrthogonalSeq2, Wide::kRightEighthBankToOrthogonalSeq3, Wide::kRightEighthBankToOrthogonalSeq4 }}, // RightEighthBankToOrthogonal
        {{ Narrow::kDiagFlatSeq0, Narrow::kDiagFlatSeq1, Narrow::kDiagFlatSeq2, Narrow::kDiagFlatSeq3 }}, // DiagFlat
        {{ Narrow::kDiagUp25Seq0, Narrow::kDiagUp25Seq1, Narrow::kDiagUp25Seq2, Narrow::kDiagUp25Seq3 }}, // DiagUp25
        {{ Narrow::kDiagUp60Seq0, Narrow::kDiagUp60Seq1, Narrow::kDiagUp60Seq2, Narrow::kDiagUp60Seq3 }}, // DiagUp60
        {{ Narrow::kDiagFlatToUp25Seq0, Narrow::kDiagFlatToUp25Seq1, Narrow::kDiagFlatToUp25Seq2, Narrow::kDiagFlatToUp25Seq3 }}, // DiagFlatToUp25
        {{ Narrow::kDiagUp25ToUp60Seq0, Narrow::kDiagUp25ToUp60Seq1, Narrow::kDiagUp25ToUp60Seq2, Narrow::kDiagUp25ToUp60Seq3 }}, // DiagUp25ToUp60
        {{ Narrow::kDiagUp60ToUp25Seq0, Narrow::kDiagUp60ToUp25Seq1, Narrow::kDiagUp60ToUp25Seq2, Narrow::kDiagUp60ToUp25Seq3 }}, // DiagUp60ToUp25
        {{ Narrow::kDiagUp25ToFlatSeq0, Narrow::kDiagUp25ToFlatSeq1, Narrow::kDiagUp25ToFlatSeq2, Narrow::kDiagUp25ToFlatSeq3 }}, // DiagUp25ToFlat
        {{ Narrow::kDiagDown25Seq0, Narrow::kDiagDown25Seq1, Narrow::kDiagDown25Seq2, Narrow::kDiagDown25Seq3 }}, // DiagDown25
        {{ Narrow::kDiagDown60Seq0, Narrow::kDiagDown60Seq1, Narrow::kDiagDown60Seq2, Narrow::kDiagDown60Seq3 }}, // DiagDown60
        {{ Narrow::kDiagFlatToDown25Seq0, Narrow::kDiagFlatToDown25Seq1, Narrow::kDiagFlatToDown25Seq2, Narrow::kDiagFlatToDown25Seq3 }}, // DiagFlatToDown25
        {{ Narrow::kDiagDown25ToDown60Seq0, Narrow::kDiagDown25ToDown60Seq1, Narrow::kDiagDown25ToDown60Seq2, Narrow::kDiagDown25ToDown60Seq3 }}, // DiagDown25ToDown60
        {{ Narrow::kDiagDown60ToDown25Seq0, Narrow::kDiagDown60ToDown25Seq1, Narrow::kDiagDown60ToDown25Seq2, Narrow::kDiagDown60ToDown25Seq3 }}, // DiagDown60ToDown25
        {{ Narrow::kDiagDown25ToFlatSeq0, Narrow::kDiagDown25ToFlatSeq1, Narrow::kDiagDown25ToFlatSeq2, Narrow::kDiagDown25ToFlatSeq3 }}, // DiagDown25ToFlat
        {{ Narrow::kDiagFlatToUp60Seq0, Narrow::kDiagFlatToUp60Seq1, Narrow::kDiagFlatToUp60Seq2, Narrow::kDiagFlatToUp60Seq3 }}, // DiagFlatToUp60
        {{ Narrow::kDiagUp60ToFlatSeq0, Narrow::kDiagUp60ToFlatSeq1, Narrow::kDiagUp60ToFlatSeq2, Narrow::kDiagUp60ToFlatSeq3 }}, // DiagUp60ToFlat
        {{ Narrow::kDiagFlatToDown60Seq0, Narrow::kDiagFlatToDown60Seq1, Narrow::kDiagFlatToDown60Seq2, Narrow::kDiagFlatToDown60Seq3 }}, // DiagFlatToDown60
        {{ Narrow::kDiagDown60ToFlatSeq0, Narrow::kDiagDown60ToFlatSeq1, Narrow::kDiagDown60ToFlatSeq2, Narrow::kDiagDown60ToFlatSeq3 }}, // DiagDown60ToFlat
        {{ Narrow::kDiagFlatToLeftBankSeq0, Narrow::kDiagFlatToLeftBankSeq1, Narrow::kDiagFlatToLeftBankSeq2, Narrow::kDiagFlatToLeftBankSeq3 }}, // DiagFlatToLeftBank
        {{ Narrow::kDiagFlatToRightBankSeq0, Narrow::kDiagFlatToRightBankSeq1, Narrow::kDiagFlatToRightBankSeq2, Narrow::kDiagFlatToRightBankSeq3 }}, // DiagFlatToRightBank
        {{ Narrow::kDiagLeftBankToFlatSeq0, Narrow::kDiagLeftBankToFlatSeq1, Narrow::kDiagLeftBankToFlatSeq2, Narrow::kDiagLeftBankToFlatSeq3 }}, // DiagLeftBankToFlat
        {{ Narrow::kDiagRightBankToFlatSeq0, Narrow::kDiagRightBankToFlatSeq1, Narrow::kDiagRightBankToFlatSeq2, Narrow::kDiagRightBankToFlatSeq3 }}, // DiagRightBankToFlat
        {{ Narrow::kDiagLeftBankToUp25Seq0, Narrow::kDiagLeftBankToUp25Seq1, Narrow::kDiagLeftBankToUp25Seq2, Narrow::kDiagLeftBankToUp25Seq3 }}, // DiagLeftBankToUp25
        {{ Narrow::kDiagRightBankToUp25Seq0, Narrow::kDiagRightBankToUp25Seq1, Narrow::kDiagRightBankToUp25Seq2, Narrow::kDiagRightBankToUp25Seq3 }}, // DiagRightBankToUp25
        {{ Narrow::kDiagUp25ToLeftBankSeq0, Narrow::kDiagUp25ToLeftBankSeq1, Narrow::kDiagUp25ToLeftBankSeq2, Narrow::kDiagUp25ToLeftBankSeq3 }}, // DiagUp25ToLeftBank
        {{ Narrow::kDiagUp25ToRightBankSeq0, Narrow::kDiagUp25ToRightBankSeq1, Narrow::kDiagUp25ToRightBankSeq2, Narrow::kDiagUp25ToRightBankSeq3 }}, // DiagUp25ToRightBank
        {{ Narrow::kDiagLeftBankToDown25Seq0, Narrow::kDiagLeftBankToDown25Seq1, Narrow::kDiagLeftBankToDown25Seq2, Narrow::kDiagLeftBankToDown25Seq3 }}, // DiagLeftBankToDown25
        {{ Narrow::kDiagRightBankToDown25Seq0, Narrow::kDiagRightBankToDown25Seq1, Narrow::kDiagRightBankToDown25Seq2, Narrow::kDiagRightBankToDown25Seq3 }}, // DiagRightBankToDown25
        {{ Narrow::kDiagDown25ToLeftBankSeq0, Narrow::kDiagDown25ToLeftBankSeq1, Narrow::kDiagDown25ToLeftBankSeq2, Narrow::kDiagDown25ToLeftBankSeq3 }}, // DiagDown25ToLeftBank
        {{ Narrow::kDiagDown25ToRightBankSeq0, Narrow::kDiagDown25ToRightBankSeq1, Narrow::kDiagDown25ToRightBankSeq2, Narrow::kDiagDown25ToRightBankSeq3 }}, // DiagDown25ToRightBank
        {{ Narrow::kDiagLeftBankSeq0, Narrow::kDiagLeftBankSeq1, Narrow::kDiagLeftBankSeq2, Narrow::kDiagLeftBankSeq3 }}, // DiagLeftBank
        {{ Narrow::kDiagRightBankSeq0, Narrow::kDiagRightBankSeq1, Narrow::kDiagRightBankSeq2, Narrow::kDiagRightBankSeq3 }}, // DiagRightBank
        {{ Wide::kLogFlumeReverserSeq0 }}, // LogFlumeReverser
        {{ Wide::kSpinningTunnelSeq0 }}, // SpinningTunnel
        {{ Narrow::kLeftBarrelRollUpToDownSeq0, Narrow::kLeftBarrelRollUpToDownSeq1, Narrow::kLeftBarrelRollUpToDownSeq2 }}, // LeftBarrelRollUpToDown
        {{ Narrow::kRightBarrelRollUpToDownSeq0, Narrow::kRightBarrelRollUpToDownSeq1, Narrow::kRightBarrelRollUpToDownSeq2 }}, // RightBarrelRollUpToDown
        {{ Narrow::kLeftBarrelRollDownToUpSeq0, Narrow::kLeftBarrelRollDownToUpSeq1, Narrow::kLeftBarrelRollDownToUpSeq2 }}, // LeftBarrelRollDownToUp
        {{ Narrow::kRightBarrelRollDownToUpSeq0, Narrow::kRightBarrelRollDownToUpSeq1, Narrow::kRightBarrelRollDownToUpSeq2 }}, // RightBarrelRollDownToUp
        {{ Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq0, Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq1, Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq2, Narrow::kLeftBankToLeftQuarterTurn3TilesUp25Seq3 }}, // LeftBankToLeftQuarterTurn3TilesUp25
        {{ Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq0, Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq1, Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq2, Narrow::kRightBankToRightQuarterTurn3TilesUp25Seq3 }}, // RightBankToRightQuarterTurn3TilesUp25
        {{ Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq1, Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, Narrow::kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 }}, // LeftQuarterTurn3TilesDown25ToLeftBank
        {{ Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq0, Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq1, Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq2, Narrow::kRightQuarterTurn3TilesDown25ToRightBankSeq3 }}, // RightQuarterTurn3TilesDown25ToRightBank
        {{ Wide::kPoweredLiftSeq0 }}, // PoweredLift
        {{ Narrow::kLeftLargeHalfLoopUpSeq0, Narrow::kLeftLargeHalfLoopUpSeq1, Narrow::kLeftLargeHalfLoopUpSeq2, Narrow::kLeftLargeHalfLoopUpSeq3, Narrow::kLeftLargeHalfLoopUpSeq4, Narrow::kLeftLargeHalfLoopUpSeq5, Narrow::kLeftLargeHalfLoopUpSeq6 }}, // LeftLargeHalfLoopUp
        {{ Narrow::kRightLargeHalfLoopUpSeq0, Narrow::kRightLargeHalfLoopUpSeq1, Narrow::kRightLargeHalfLoopUpSeq2, Narrow::kRightLargeHalfLoopUpSeq3, Narrow::kRightLargeHalfLoopUpSeq4, Narrow::kRightLargeHalfLoopUpSeq5, Narrow::kRightLargeHalfLoopUpSeq6 }}, // RightLargeHalfLoopUp
        {{ Narrow::kLeftLargeHalfLoopDownSeq0, Narrow::kLeftLargeHalfLoopDownSeq1, Narrow::kLeftLargeHalfLoopDownSeq2, Narrow::kLeftLargeHalfLoopDownSeq3, Narrow::kLeftLargeHalfLoopDownSeq4, Narrow::kLeftLargeHalfLoopDownSeq5, Narrow::kLeftLargeHalfLoopDownSeq6 }}, // LeftLargeHalfLoopDown
        {{ Narrow::kRightLargeHalfLoopDownSeq0, Narrow::kRightLargeHalfLoopDownSeq1, Narrow::kRightLargeHalfLoopDownSeq2, Narrow::kRightLargeHalfLoopDownSeq3, Narrow::kRightLargeHalfLoopDownSeq4, Narrow::kRightLargeHalfLoopDownSeq5, Narrow::kRightLargeHalfLoopDownSeq6 }}, // RightLargeHalfLoopDown
        {{ Narrow::kLeftFlyerTwistUpSeq0, Narrow::kLeftFlyerTwistUpSeq1, Narrow::kLeftFlyerTwistUpSeq2 }}, // LeftFlyerTwistUp
        {{ Narrow::kRightFlyerTwistUpSeq0, Narrow::kRightFlyerTwistUpSeq1, Narrow::kRightFlyerTwistUpSeq2 }}, // RightFlyerTwistUp
        {{ Narrow::kLeftFlyerTwistDownSeq0, Narrow::kLeftFlyerTwistDownSeq1, Narrow::kLeftFlyerTwistDownSeq2 }}, // LeftFlyerTwistDown
        {{ Narrow::kRightFlyerTwistDownSeq0, Narrow::kRightFlyerTwistDownSeq1, Narrow::kRightFlyerTwistDownSeq2 }}, // RightFlyerTwistDown
        {{ Narrow::kFlyerHalfLoopUninvertedUpSeq0, Narrow::kFlyerHalfLoopUninvertedUpSeq1, Narrow::kFlyerHalfLoopUninvertedUpSeq2, Narrow::kFlyerHalfLoopUninvertedUpSeq3 }}, // FlyerHalfLoopUninvertedUp
        {{ Narrow::kFlyerHalfLoopInvertedDownSeq0, Narrow::kFlyerHalfLoopInvertedDownSeq1, Narrow::kFlyerHalfLoopInvertedDownSeq2, Narrow::kFlyerHalfLoopInvertedDownSeq3 }}, // FlyerHalfLoopInvertedDown
        {{ Narrow::kLeftFlyerCorkscrewUpSeq0, Narrow::kLeftFlyerCorkscrewUpSeq1, Narrow::kLeftFlyerCorkscrewUpSeq2 }}, // LeftFlyerCorkscrewUp
        {{ Narrow::kRightFlyerCorkscrewUpSeq0, Narrow::kRightFlyerCorkscrewUpSeq1, Narrow::kRightFlyerCorkscrewUpSeq2 }}, // RightFlyerCorkscrewUp
        {{ Narrow::kLeftFlyerCorkscrewDownSeq0, Narrow::kLeftFlyerCorkscrewDownSeq1, Narrow::kLeftFlyerCorkscrewDownSeq2 }}, // LeftFlyerCorkscrewDown
        {{ Narrow::kRightFlyerCorkscrewDownSeq0, Narrow::kRightFlyerCorkscrewDownSeq1, Narrow::kRightFlyerCorkscrewDownSeq2 }}, // RightFlyerCorkscrewDown
        {{ Narrow::kHeartLineTransferUpSeq0, Narrow::kHeartLineTransferUpSeq1, Narrow::kHeartLineTransferUpSeq2, Narrow::kHeartLineTransferUpSeq3 }}, // HeartLineTransferUp
        {{ Narrow::kHeartLineTransferDownSeq0, Narrow::kHeartLineTransferDownSeq1, Narrow::kHeartLineTransferDownSeq2, Narrow::kHeartLineTransferDownSeq3 }}, // HeartLineTransferDown
        {{ Narrow::kLeftHeartLineRollSeq0, Narrow::kLeftHeartLineRollSeq1, Narrow::kLeftHeartLineRollSeq2, Narrow::kLeftHeartLineRollSeq3, Narrow::kLeftHeartLineRollSeq4, Narrow::kLeftHeartLineRollSeq5 }}, // LeftHeartLineRoll
        {{ Narrow::kRightHeartLineRollSeq0, Narrow::kRightHeartLineRollSeq1, Narrow::kRightHeartLineRollSeq2, Narrow::kRightHeartLineRollSeq3, Narrow::kRightHeartLineRollSeq4, Narrow::kRightHeartLineRollSeq5 }}, // RightHeartLineRoll
        {{ Narrow::kMinigolfHoleASeq0, Narrow::kMinigolfHoleASeq1 }}, // MinigolfHoleA
        {{ Narrow::kMinigolfHoleBSeq0, Narrow::kMinigolfHoleBSeq1 }}, // MinigolfHoleB
        {{ Narrow::kMinigolfHoleCSeq0, Narrow::kMinigolfHoleCSeq1 }}, // MinigolfHoleC
        {{ Narrow::kMinigolfHoleDSeq0, Narrow::kMinigolfHoleDSeq1, Narrow::kMinigolfHoleDSeq2 }}, // MinigolfHoleD
        {{ Narrow::kMinigolfHoleESeq0, Narrow::kMinigolfHoleESeq1, Narrow::kMinigolfHoleESeq2 }}, // MinigolfHoleE
        {{ Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq0, Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq1, Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq2, Wide::kMultiDimInvertedFlatToDown90QuarterLoopSeq3 }}, // MultiDimInvertedFlatToDown90QuarterLoop
        {{ Narrow::kUp90ToInvertedFlatQuarterLoopSeq0, Narrow::kUp90ToInvertedFlatQuarterLoopSeq1, Narrow::kUp90ToInvertedFlatQuarterLoopSeq2 }}, // Up90ToInvertedFlatQuarterLoop
        {{ Narrow::kInvertedFlatToDown90QuarterLoopSeq0, Narrow::kInvertedFlatToDown90QuarterLoopSeq1, Narrow::kInvertedFlatToDown90QuarterLoopSeq2, Narrow::kInvertedFlatToDown90QuarterLoopSeq3 }}, // InvertedFlatToDown90QuarterLoop
        {{ Narrow::kLeftCurvedLiftHillSeq0, Narrow::kLeftCurvedLiftHillSeq1, Narrow::kLeftCurvedLiftHillSeq2, Narrow::kLeftCurvedLiftHillSeq3 }}, // LeftCurvedLiftHill
        {{ Narrow::kRightCurvedLiftHillSeq0, Narrow::kRightCurvedLiftHillSeq1, Narrow::kRightCurvedLiftHillSeq2, Narrow::kRightCurvedLiftHillSeq3 }}, // RightCurvedLiftHill
        {{ Narrow::kLeftReverserSeq0, Narrow::kLeftReverserSeq1, Narrow::kLeftReverserSeq2, Narrow::kLeftReverserSeq3, Narrow::kLeftReverserSeq4, Narrow::kLeftReverserSeq5 }}, // LeftReverser
        {{ Narrow::kRightReverserSeq0, Narrow::kRightReverserSeq1, Narrow::kRightReverserSeq2, Narrow::kRightReverserSeq3, Narrow::kRightReverserSeq4, Narrow::kRightReverserSeq5 }}, // RightReverser
        {{ Narrow::kAirThrustTopCapSeq0, Narrow::kAirThrustTopCapSeq1, Narrow::kAirThrustTopCapSeq2, Narrow::kAirThrustTopCapSeq3 }}, // AirThrustTopCap
        {{ Narrow::kAirThrustVerticalDownSeq0, Narrow::kAirThrustVerticalDownSeq1 }}, // AirThrustVerticalDown
        {{ Narrow::kAirThrustVerticalDownToLevelSeq0, Narrow::kAirThrustVerticalDownToLevelSeq1, Narrow::kAirThrustVerticalDownToLevelSeq2, Narrow::kAirThrustVerticalDownToLevelSeq3, Narrow::kAirThrustVerticalDownToLevelSeq4, Narrow::kAirThrustVerticalDownToLevelSeq5, Narrow::kAirThrustVerticalDownToLevelSeq6 }}, // AirThrustVerticalDownToLevel
        {{ Wide::kBlockBrakesSeq0 }}, // BlockBrakes
        {{ Narrow::kLeftBankedQuarterTurn3TileUp25Seq0, Narrow::kLeftBankedQuarterTurn3TileUp25Seq1, Narrow::kLeftBankedQuarterTurn3TileUp25Seq2, Narrow::kLeftBankedQuarterTurn3TileUp25Seq3 }}, // LeftBankedQuarterTurn3TileUp25
        {{ Narrow::kRightBankedQuarterTurn3TileUp25Seq0, Narrow::kRightBankedQuarterTurn3TileUp25Seq1, Narrow::kRightBankedQuarterTurn3TileUp25Seq2, Narrow::kRightBankedQuarterTurn3TileUp25Seq3 }}, // RightBankedQuarterTurn3TileUp25
        {{ Narrow::kLeftBankedQuarterTurn3TileDown25Seq0, Narrow::kLeftBankedQuarterTurn3TileDown25Seq1, Narrow::kLeftBankedQuarterTurn3TileDown25Seq2, Narrow::kLeftBankedQuarterTurn3TileDown25Seq3 }}, // LeftBankedQuarterTurn3TileDown25
        {{ Narrow::kRightBankedQuarterTurn3TileDown25Seq0, Narrow::kRightBankedQuarterTurn3TileDown25Seq1, Narrow::kRightBankedQuarterTurn3TileDown25Seq2, Narrow::kRightBankedQuarterTurn3TileDown25Seq3 }}, // RightBankedQuarterTurn3TileDown25
        {{ Narrow::kLeftBankedQuarterTurn5TileUp25Seq0, Narrow::kLeftBankedQuarterTurn5TileUp25Seq1, Narrow::kLeftBankedQuarterTurn5TileUp25Seq2, Narrow::kLeftBankedQuarterTurn5TileUp25Seq3, Narrow::kLeftBankedQuarterTurn5TileUp25Seq4, Narrow::kLeftBankedQuarterTurn5TileUp25Seq5, Narrow::kLeftBankedQuarterTurn5TileUp25Seq6 }}, // LeftBankedQuarterTurn5TileUp25
        {{ Narrow::kRightBankedQuarterTurn5TileUp25Seq0, Narrow::kRightBankedQuarterTurn5TileUp25Seq1, Narrow::kRightBankedQuarterTurn5TileUp25Seq2, Narrow::kRightBankedQuarterTurn5TileUp25Seq3, Narrow::kRightBankedQuarterTurn5TileUp25Seq4, Narrow::kRightBankedQuarterTurn5TileUp25Seq5, Narrow::kRightBankedQuarterTurn5TileUp25Seq6 }}, // RightBankedQuarterTurn5TileUp25
        {{ Narrow::kLeftBankedQuarterTurn5TileDown25Seq0, Narrow::kLeftBankedQuarterTurn5TileDown25Seq1, Narrow::kLeftBankedQuarterTurn5TileDown25Seq2, Narrow::kLeftBankedQuarterTurn5TileDown25Seq3, Narrow::kLeftBankedQuarterTurn5TileDown25Seq4, Narrow::kLeftBankedQuarterTurn5TileDown25Seq5, Narrow::kLeftBankedQuarterTurn5TileDown25Seq6 }}, // LeftBankedQuarterTurn5TileDown25
        {{ Narrow::kRightBankedQuarterTurn5TileDown25Seq0, Narrow::kRightBankedQuarterTurn5TileDown25Seq1, Narrow::kRightBankedQuarterTurn5TileDown25Seq2, Narrow::kRightBankedQuarterTurn5TileDown25Seq3, Narrow::kRightBankedQuarterTurn5TileDown25Seq4, Narrow::kRightBankedQuarterTurn5TileDown25Seq5, Narrow::kRightBankedQuarterTurn5TileDown25Seq6 }}, // RightBankedQuarterTurn5TileDown25
        {{ Narrow::kUp25ToLeftBankedUp25Seq0 }}, // Up25ToLeftBankedUp25
        {{ Narrow::kUp25ToRightBankedUp25Seq0 }}, // Up25ToRightBankedUp25
        {{ Narrow::kLeftBankedUp25ToUp25Seq0 }}, // LeftBankedUp25ToUp25
        {{ Narrow::kRightBankedUp25ToUp25Seq0 }}, // RightBankedUp25ToUp25
        {{ Narrow::kDown25ToLeftBankedDown25Seq0 }}, // Down25ToLeftBankedDown25
        {{ Narrow::kDown25ToRightBankedDown25Seq0 }}, // Down25ToRightBankedDown25
        {{ Narrow::kLeftBankedDown25ToDown25Seq0 }}, // LeftBankedDown25ToDown25
        {{ Narrow::kRightBankedDown25ToDown25Seq0 }}, // RightBankedDown25ToDown25
        {{ Narrow::kLeftBankedFlatToLeftBankedUp25Seq0 }}, // LeftBankedFlatToLeftBankedUp25
        {{ Narrow::kRightBankedFlatToRightBankedUp25Seq0 }}, // RightBankedFlatToRightBankedUp25
        {{ Narrow::kLeftBankedUp25ToLeftBankedFlatSeq0 }}, // LeftBankedUp25ToLeftBankedFlat
        {{ Narrow::kRightBankedUp25ToRightBankedFlatSeq0 }}, // RightBankedUp25ToRightBankedFlat
        {{ Narrow::kLeftBankedFlatToLeftBankedDown25Seq0 }}, // LeftBankedFlatToLeftBankedDown25
        {{ Narrow::kRightBankedFlatToRightBankedDown25Seq0 }}, // RightBankedFlatToRightBankedDown25
        {{ Narrow::kLeftBankedDown25ToLeftBankedFlatSeq0 }}, // LeftBankedDown25ToLeftBankedFlat
        {{ Narrow::kRightBankedDown25ToRightBankedFlatSeq0 }}, // RightBankedDown25ToRightBankedFlat
        {{ Narrow::kFlatToLeftBankedUp25Seq0 }}, // FlatToLeftBankedUp25
        {{ Narrow::kFlatToRightBankedUp25Seq0 }}, // FlatToRightBankedUp25
        {{ Narrow::kLeftBankedUp25ToFlatSeq0 }}, // LeftBankedUp25ToFlat
        {{ Narrow::kRightBankedUp25ToFlatSeq0 }}, // RightBankedUp25ToFlat
        {{ Narrow::kFlatToLeftBankedDown25Seq0 }}, // FlatToLeftBankedDown25
        {{ Narrow::kFlatToRightBankedDown25Seq0 }}, // FlatToRightBankedDown25
        {{ Narrow::kLeftBankedDown25ToFlatSeq0 }}, // LeftBankedDown25ToFlat
        {{ Narrow::kRightBankedDown25ToFlatSeq0 }}, // RightBankedDown25ToFlat
        {{ Narrow::kLeftQuarterTurn1TileUp90Seq0, Narrow::kLeftQuarterTurn1TileUp90Seq1 }}, // LeftQuarterTurn1TileUp90
        {{ Narrow::kRightQuarterTurn1TileUp90Seq0, Narrow::kRightQuarterTurn1TileUp90Seq1 }}, // RightQuarterTurn1TileUp90
        {{ Narrow::kLeftQuarterTurn1TileDown90Seq0, Narrow::kLeftQuarterTurn1TileDown90Seq1 }}, // LeftQuarterTurn1TileDown90
        {{ Narrow::kRightQuarterTurn1TileDown90Seq0, Narrow::kRightQuarterTurn1TileDown90Seq1 }}, // RightQuarterTurn1TileDown90
        {{ Wide::kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, Wide::kMultiDimUp90ToInvertedFlatQuarterLoopSeq1, Wide::kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 }}, // MultiDimUp90ToInvertedFlatQuarterLoop
        {{ Wide::kMultiDimFlatToDown90QuarterLoopSeq0, Wide::kMultiDimFlatToDown90QuarterLoopSeq1, Wide::kMultiDimFlatToDown90QuarterLoopSeq2, Wide::kMultiDimFlatToDown90QuarterLoopSeq3 }}, // MultiDimFlatToDown90QuarterLoop
        {{ Wide::kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, Wide::kMultiDimInvertedUp90ToFlatQuarterLoopSeq1, Wide::kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 }}, // MultiDimInvertedUp90ToFlatQuarterLoop
        {{ Narrow::kRotationControlToggleSeq0 }}, // RotationControlToggle
        {{ Narrow::kFlatTrack1x4ASeq0, Narrow::kFlatTrack1x4ASeq1, Narrow::kFlatTrack1x4ASeq2, Narrow::kFlatTrack1x4ASeq3 }}, // FlatTrack1x4A
        {{ Narrow::kFlatTrack2x2Seq0, Narrow::kFlatTrack2x2Seq1, Narrow::kFlatTrack2x2Seq2, Narrow::kFlatTrack2x2Seq3 }}, // FlatTrack2x2
        {{ Narrow::kFlatTrack4x4Seq0, Narrow::kFlatTrack4x4Seq1, Narrow::kFlatTrack4x4Seq2, Narrow::kFlatTrack4x4Seq3, Narrow::kFlatTrack4x4Seq4, Narrow::kFlatTrack4x4Seq5, Narrow::kFlatTrack4x4Seq6, Narrow::kFlatTrack4x4Seq7, Narrow::kFlatTrack4x4Seq8, Narrow::kFlatTrack4x4Seq9, Narrow::kFlatTrack4x4Seq10, Narrow::kFlatTrack4x4Seq11, Narrow::kFlatTrack4x4Seq12, Narrow::kFlatTrack4x4Seq13, Narrow::kFlatTrack4x4Seq14, Narrow::kFlatTrack4x4Seq15 }}, // FlatTrack4x4
        {{ Narrow::kFlatTrack2x4Seq0, Narrow::kFlatTrack2x4Seq1, Narrow::kFlatTrack2x4Seq2, Narrow::kFlatTrack2x4Seq3, Narrow::kFlatTrack2x4Seq4, Narrow::kFlatTrack2x4Seq5, Narrow::kFlatTrack2x4Seq6, Narrow::kFlatTrack2x4Seq7 }}, // FlatTrack2x4
        {{ Narrow::kFlatTrack1x5Seq0, Narrow::kFlatTrack1x5Seq1, Narrow::kFlatTrack1x5Seq2, Narrow::kFlatTrack1x5Seq3, Narrow::kFlatTrack1x5Seq4 }}, // FlatTrack1x5
        {{ Narrow::kFlatTrack1x1ASeq0 }}, // FlatTrack1x1A
        {{ Narrow::kFlatTrack1x4BSeq0, Narrow::kFlatTrack1x4BSeq1, Narrow::kFlatTrack1x4BSeq2, Narrow::kFlatTrack1x4BSeq3 }}, // FlatTrack1x4B
        {{ Narrow::kFlatTrack1x1BSeq0 }}, // FlatTrack1x1B
        {{ Narrow::kFlatTrack1x4CSeq0, Narrow::kFlatTrack1x4CSeq1, Narrow::kFlatTrack1x4CSeq2, Narrow::kFlatTrack1x4CSeq3 }}, // FlatTrack1x4C
        {{ Narrow::kFlatTrack3x3Seq0, Narrow::kFlatTrack3x3Seq1, Narrow::kFlatTrack3x3Seq2, Narrow::kFlatTrack3x3Seq3, Narrow::kFlatTrack3x3Seq4, Narrow::kFlatTrack3x3Seq5, Narrow::kFlatTrack3x3Seq6, Narrow::kFlatTrack3x3Seq7, Narrow::kFlatTrack3x3Seq8 }}, // FlatTrack3x3
        {{ Narrow::kLeftLargeCorkscrewUpSeq0, Narrow::kLeftLargeCorkscrewUpSeq1, Narrow::kLeftLargeCorkscrewUpSeq2, Narrow::kLeftLargeCorkscrewUpSeq3, Narrow::kLeftLargeCorkscrewUpSeq4, Narrow::kLeftLargeCorkscrewUpSeq5 }}, // LeftLargeCorkscrewUp
        {{ Narrow::kRightLargeCorkscrewUpSeq0, Narrow::kRightLargeCorkscrewUpSeq1, Narrow::kRightLargeCorkscrewUpSeq2, Narrow::kRightLargeCorkscrewUpSeq3, Narrow::kRightLargeCorkscrewUpSeq4, Narrow::kRightLargeCorkscrewUpSeq5 }}, // RightLargeCorkscrewUp
        {{ Narrow::kLeftLargeCorkscrewDownSeq0, Narrow::kLeftLargeCorkscrewDownSeq1, Narrow::kLeftLargeCorkscrewDownSeq2, Narrow::kLeftLargeCorkscrewDownSeq3, Narrow::kLeftLargeCorkscrewDownSeq4, Narrow::kLeftLargeCorkscrewDownSeq5 }}, // LeftLargeCorkscrewDown
        {{ Narrow::kRightLargeCorkscrewDownSeq0, Narrow::kRightLargeCorkscrewDownSeq1, Narrow::kRightLargeCorkscrewDownSeq2, Narrow::kRightLargeCorkscrewDownSeq3, Narrow::kRightLargeCorkscrewDownSeq4, Narrow::kRightLargeCorkscrewDownSeq5 }}, // RightLargeCorkscrewDown
        {{ Narrow::kLeftMediumHalfLoopUpSeq0, Narrow::kLeftMediumHalfLoopUpSeq1, Narrow::kLeftMediumHalfLoopUpSeq2, Narrow::kLeftMediumHalfLoopUpSeq3, Narrow::kLeftMediumHalfLoopUpSeq4 }}, // LeftMediumHalfLoopUp
        {{ Narrow::kRightMediumHalfLoopUpSeq0, Narrow::kRightMediumHalfLoopUpSeq1, Narrow::kRightMediumHalfLoopUpSeq2, Narrow::kRightMediumHalfLoopUpSeq3, Narrow::kRightMediumHalfLoopUpSeq4 }}, // RightMediumHalfLoopUp
        {{ Narrow::kLeftMediumHalfLoopDownSeq0, Narrow::kLeftMediumHalfLoopDownSeq1, Narrow::kLeftMediumHalfLoopDownSeq2, Narrow::kLeftMediumHalfLoopDownSeq3, Narrow::kLeftMediumHalfLoopDownSeq4 }}, // LeftMediumHalfLoopDown
        {{ Narrow::kRightMediumHalfLoopDownSeq0, Narrow::kRightMediumHalfLoopDownSeq1, Narrow::kRightMediumHalfLoopDownSeq2, Narrow::kRightMediumHalfLoopDownSeq3, Narrow::kRightMediumHalfLoopDownSeq4 }}, // RightMediumHalfLoopDown
        {{ Narrow::kLeftZeroGRollUpSeq0, Narrow::kLeftZeroGRollUpSeq1, Narrow::kLeftZeroGRollUpSeq2 }}, // LeftZeroGRollUp
        {{ Narrow::kRightZeroGRollUpSeq0, Narrow::kRightZeroGRollUpSeq1, Narrow::kRightZeroGRollUpSeq2 }}, // RightZeroGRollUp
        {{ Narrow::kLeftZeroGRollDownSeq0, Narrow::kLeftZeroGRollDownSeq1, Narrow::kLeftZeroGRollDownSeq2 }}, // LeftZeroGRollDown
        {{ Narrow::kRightZeroGRollDownSeq0, Narrow::kRightZeroGRollDownSeq1, Narrow::kRightZeroGRollDownSeq2 }}, // RightZeroGRollDown
        {{ Narrow::kLeftLargeZeroGRollUpSeq0, Narrow::kLeftLargeZeroGRollUpSeq1, Narrow::kLeftLargeZeroGRollUpSeq2, Narrow::kLeftLargeZeroGRollUpSeq3 }}, // LeftLargeZeroGRollUp
        {{ Narrow::kRightLargeZeroGRollUpSeq0, Narrow::kRightLargeZeroGRollUpSeq1, Narrow::kRightLargeZeroGRollUpSeq2, Narrow::kRightLargeZeroGRollUpSeq3 }}, // RightLargeZeroGRollUp
        {{ Narrow::kLeftLargeZeroGRollDownSeq0, Narrow::kLeftLargeZeroGRollDownSeq1, Narrow::kLeftLargeZeroGRollDownSeq2, Narrow::kLeftLargeZeroGRollDownSeq3 }}, // LeftLargeZeroGRollDown
        {{ Narrow::kRightLargeZeroGRollDownSeq0, Narrow::kRightLargeZeroGRollDownSeq1, Narrow::kRightLargeZeroGRollDownSeq2, Narrow::kRightLargeZeroGRollDownSeq3 }}, // RightLargeZeroGRollDown
        {{ Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq0, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq1, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq2, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq3, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq4, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq5, Narrow::kLeftFlyerLargeHalfLoopUninvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedUp
        {{ Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq0, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq1, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq2, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq3, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq4, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq5, Narrow::kRightFlyerLargeHalfLoopUninvertedUpSeq6 }}, // RightFlyerLargeHalfLoopUninvertedUp
        {{ Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq0, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq1, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq2, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq3, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq4, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq5, Narrow::kLeftFlyerLargeHalfLoopInvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopInvertedDown
        {{ Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq0, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq1, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq2, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq3, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq4, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq5, Narrow::kRightFlyerLargeHalfLoopInvertedDownSeq6 }}, // RightFlyerLargeHalfLoopInvertedDown
        {{ Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq0, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq1, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq2, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq3, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq4, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq5, Narrow::kLeftFlyerLargeHalfLoopInvertedUpSeq6 }}, // LeftFlyerLargeHalfLoopInvertedUp
        {{ Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq0, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq1, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq2, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq3, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq4, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq5, Narrow::kRightFlyerLargeHalfLoopInvertedUpSeq6 }}, // RightFlyerLargeHalfLoopInvertedUp
        {{ Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq0, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq1, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq2, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq3, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq4, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq5, Narrow::kLeftFlyerLargeHalfLoopUninvertedDownSeq6 }}, // LeftFlyerLargeHalfLoopUninvertedDown
        {{ Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq0, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq1, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq2, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq3, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq4, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq5, Narrow::kRightFlyerLargeHalfLoopUninvertedDownSeq6 }}, // RightFlyerLargeHalfLoopUninvertedDown
        {{ Narrow::kFlyerHalfLoopInvertedUpSeq0, Narrow::kFlyerHalfLoopInvertedUpSeq1, Narrow::kFlyerHalfLoopInvertedUpSeq2, Narrow::kFlyerHalfLoopInvertedUpSeq3 }}, // FlyerHalfLoopInvertedUp
        {{ Narrow::kFlyerHalfLoopUninvertedDownSeq0, Narrow::kFlyerHalfLoopUninvertedDownSeq1, Narrow::kFlyerHalfLoopUninvertedDownSeq2, Narrow::kFlyerHalfLoopUninvertedDownSeq3 }}, // FlyerHalfLoopUninvertedDown
        {{ Narrow::kLeftEighthToDiagUp25Seq0, Narrow::kLeftEighthToDiagUp25Seq1, Narrow::kLeftEighthToDiagUp25Seq2, Narrow::kLeftEighthToDiagUp25Seq3, Narrow::kLeftEighthToDiagUp25Seq4 }}, // LeftEighthToDiagUp25
        {{ Narrow::kRightEighthToDiagUp25Seq0, Narrow::kRightEighthToDiagUp25Seq1, Narrow::kRightEighthToDiagUp25Seq2, Narrow::kRightEighthToDiagUp25Seq3, Narrow::kRightEighthToDiagUp25Seq4 }}, // RightEighthToDiagUp25
        {{ Narrow::kLeftEighthToDiagDown25Seq0, Narrow::kLeftEighthToDiagDown25Seq1, Narrow::kLeftEighthToDiagDown25Seq2, Narrow::kLeftEighthToDiagDown25Seq3, Narrow::kLeftEighthToDiagDown25Seq4 }}, // LeftEighthToDiagDown25
        {{ Narrow::kRightEighthToDiagDown25Seq0, Narrow::kRightEighthToDiagDown25Seq1, Narrow::kRightEighthToDiagDown25Seq2, Narrow::kRightEighthToDiagDown25Seq3, Narrow::kRightEighthToDiagDown25Seq4 }}, // RightEighthToDiagDown25
        {{ Narrow::kLeftEighthToOrthogonalUp25Seq0, Narrow::kLeftEighthToOrthogonalUp25Seq1, Narrow::kLeftEighthToOrthogonalUp25Seq2, Narrow::kLeftEighthToOrthogonalUp25Seq3, Narrow::kLeftEighthToOrthogonalUp25Seq4 }}, // LeftEighthToOrthogonalUp25
        {{ Narrow::kRightEighthToOrthogonalUp25Seq0, Narrow::kRightEighthToOrthogonalUp25Seq1, Narrow::kRightEighthToOrthogonalUp25Seq2, Narrow::kRightEighthToOrthogonalUp25Seq3, Narrow::kRightEighthToOrthogonalUp25Seq4 }}, // RightEighthToOrthogonalUp25
        {{ Narrow::kLeftEighthToOrthogonalDown25Seq0, Narrow::kLeftEighthToOrthogonalDown25Seq1, Narrow::kLeftEighthToOrthogonalDown25Seq2, Narrow::kLeftEighthToOrthogonalDown25Seq3, Narrow::kLeftEighthToOrthogonalDown25Seq4 }}, // LeftEighthToOrthogonalDown25
        {{ Narrow::kRightEighthToOrthogonalDown25Seq0, Narrow::kRightEighthToOrthogonalDown25Seq1, Narrow::kRightEighthToOrthogonalDown25Seq2, Narrow::kRightEighthToOrthogonalDown25Seq3, Narrow::kRightEighthToOrthogonalDown25Seq4 }}, // RightEighthToOrthogonalDown25
        {{ Narrow::kDiagUp25ToLeftBankedUp25Seq0, Narrow::kDiagUp25ToLeftBankedUp25Seq1, Narrow::kDiagUp25ToLeftBankedUp25Seq2, Narrow::kDiagUp25ToLeftBankedUp25Seq3 }}, // DiagUp25ToLeftBankedUp25
        {{ Narrow::kDiagUp25ToRightBankedUp25Seq0, Narrow::kDiagUp25ToRightBankedUp25Seq1, Narrow::kDiagUp25ToRightBankedUp25Seq2, Narrow::kDiagUp25ToRightBankedUp25Seq3 }}, // DiagUp25ToRightBankedUp25
        {{ Narrow::kDiagLeftBankedUp25ToUp25Seq0, Narrow::kDiagLeftBankedUp25ToUp25Seq1, Narrow::kDiagLeftBankedUp25ToUp25Seq2, Narrow::kDiagLeftBankedUp25ToUp25Seq3 }}, // DiagLeftBankedUp25ToUp25
        {{ Narrow::kDiagRightBankedUp25ToUp25Seq0, Narrow::kDiagRightBankedUp25ToUp25Seq1, Narrow::kDiagRightBankedUp25ToUp25Seq2, Narrow::kDiagRightBankedUp25ToUp25Seq3 }}, // DiagRightBankedUp25ToUp25
        {{ Narrow::kDiagDown25ToLeftBankedDown25Seq0, Narrow::kDiagDown25ToLeftBankedDown25Seq1, Narrow::kDiagDown25ToLeftBankedDown25Seq2, Narrow::kDiagDown25ToLeftBankedDown25Seq3 }}, // DiagDown25ToLeftBankedDown25
        {{ Narrow::kDiagDown25ToRightBankedDown25Seq0, Narrow::kDiagDown25ToRightBankedDown25Seq1, Narrow::kDiagDown25ToRightBankedDown25Seq2, Narrow::kDiagDown25ToRightBankedDown25Seq3 }}, // DiagDown25ToRightBankedDown25
        {{ Narrow::kDiagLeftBankedDown25ToDown25Seq0, Narrow::kDiagLeftBankedDown25ToDown25Seq1, Narrow::kDiagLeftBankedDown25ToDown25Seq2, Narrow::kDiagLeftBankedDown25ToDown25Seq3 }}, // DiagLeftBankedDown25ToDown25
        {{ Narrow::kDiagRightBankedDown25ToDown25Seq0, Narrow::kDiagRightBankedDown25ToDown25Seq1, Narrow::kDiagRightBankedDown25ToDown25Seq2, Narrow::kDiagRightBankedDown25ToDown25Seq3 }}, // DiagRightBankedDown25ToDown25
        {{ Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq0, Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq1, Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq2, Narrow::kDiagLeftBankedFlatToLeftBankedUp25Seq3 }}, // DiagLeftBankedFlatToLeftBankedUp25
        {{ Narrow::kDiagRightBankedFlatToRightBankedUp25Seq0, Narrow::kDiagRightBankedFlatToRightBankedUp25Seq1, Narrow::kDiagRightBankedFlatToRightBankedUp25Seq2, Narrow::kDiagRightBankedFlatToRightBankedUp25Seq3 }}, // DiagRightBankedFlatToRightBankedUp25
        {{ Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq0, Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq1, Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq2, Narrow::kDiagLeftBankedUp25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedUp25ToLeftBankedFlat
        {{ Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq0, Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq1, Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq2, Narrow::kDiagRightBankedUp25ToRightBankedFlatSeq3 }}, // DiagRightBankedUp25ToRightBankedFlat
        {{ Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq0, Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq1, Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq2, Narrow::kDiagLeftBankedFlatToLeftBankedDown25Seq3 }}, // DiagLeftBankedFlatToLeftBankedDown25
        {{ Narrow::kDiagRightBankedFlatToRightBankedDown25Seq0, Narrow::kDiagRightBankedFlatToRightBankedDown25Seq1, Narrow::kDiagRightBankedFlatToRightBankedDown25Seq2, Narrow::kDiagRightBankedFlatToRightBankedDown25Seq3 }}, // DiagRightBankedFlatToRightBankedDown25
        {{ Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq0, Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq1, Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq2, Narrow::kDiagLeftBankedDown25ToLeftBankedFlatSeq3 }}, // DiagLeftBankedDown25ToLeftBankedFlat
        {{ Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq0, Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq1, Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq2, Narrow::kDiagRightBankedDown25ToRightBankedFlatSeq3 }}, // DiagRightBankedDown25ToRightBankedFlat
        {{ Narrow::kDiagFlatToLeftBankedUp25Seq0, Narrow::kDiagFlatToLeftBankedUp25Seq1, Narrow::kDiagFlatToLeftBankedUp25Seq2, Narrow::kDiagFlatToLeftBankedUp25Seq3 }}, // DiagFlatToLeftBankedUp25
        {{ Narrow::kDiagFlatToRightBankedUp25Seq0, Narrow::kDiagFlatToRightBankedUp25Seq1, Narrow::kDiagFlatToRightBankedUp25Seq2, Narrow::kDiagFlatToRightBankedUp25Seq3 }}, // DiagFlatToRightBankedUp25
        {{ Narrow::kDiagLeftBankedUp25ToFlatSeq0, Narrow::kDiagLeftBankedUp25ToFlatSeq1, Narrow::kDiagLeftBankedUp25ToFlatSeq2, Narrow::kDiagLeftBankedUp25ToFlatSeq3 }}, // DiagLeftBankedUp25ToFlat
        {{ Narrow::kDiagRightBankedUp25ToFlatSeq0, Narrow::kDiagRightBankedUp25ToFlatSeq1, Narrow::kDiagRightBankedUp25ToFlatSeq2, Narrow::kDiagRightBankedUp25ToFlatSeq3 }}, // DiagRightBankedUp25ToFlat
        {{ Narrow::kDiagFlatToLeftBankedDown25Seq0, Narrow::kDiagFlatToLeftBankedDown25Seq1, Narrow::kDiagFlatToLeftBankedDown25Seq2, Narrow::kDiagFlatToLeftBankedDown25Seq3 }}, // DiagFlatToLeftBankedDown25
        {{ Narrow::kDiagFlatToRightBankedDown25Seq0, Narrow::kDiagFlatToRightBankedDown25Seq1, Narrow::kDiagFlatToRightBankedDown25Seq2, Narrow::kDiagFlatToRightBankedDown25Seq3 }}, // DiagFlatToRightBankedDown25
        {{ Narrow::kDiagLeftBankedDown25ToFlatSeq0, Narrow::kDiagLeftBankedDown25ToFlatSeq1, Narrow::kDiagLeftBankedDown25ToFlatSeq2, Narrow::kDiagLeftBankedDown25ToFlatSeq3 }}, // DiagLeftBankedDown25ToFlat
        {{ Narrow::kDiagRightBankedDown25ToFlatSeq0, Narrow::kDiagRightBankedDown25ToFlatSeq1, Narrow::kDiagRightBankedDown25ToFlatSeq2, Narrow::kDiagRightBankedDown25ToFlatSeq3 }}, // DiagRightBankedDown25ToFlat
        {{ Narrow::kDiagUp25LeftBankedSeq0, Narrow::kDiagUp25LeftBankedSeq1, Narrow::kDiagUp25LeftBankedSeq2, Narrow::kDiagUp25LeftBankedSeq3 }}, // DiagUp25LeftBanked
        {{ Narrow::kDiagUp25RightBankedSeq0, Narrow::kDiagUp25RightBankedSeq1, Narrow::kDiagUp25RightBankedSeq2, Narrow::kDiagUp25RightBankedSeq3 }}, // DiagUp25RightBanked
        {{ Narrow::kDiagDown25LeftBankedSeq0, Narrow::kDiagDown25LeftBankedSeq1, Narrow::kDiagDown25LeftBankedSeq2, Narrow::kDiagDown25LeftBankedSeq3 }}, // DiagDown25LeftBanked
        {{ Narrow::kDiagDown25RightBankedSeq0, Narrow::kDiagDown25RightBankedSeq1, Narrow::kDiagDown25RightBankedSeq2, Narrow::kDiagDown25RightBankedSeq3 }}, // DiagDown25RightBanked
        {{ Narrow::kLeftEighthBankToDiagUp25Seq0, Narrow::kLeftEighthBankToDiagUp25Seq1, Narrow::kLeftEighthBankToDiagUp25Seq2, Narrow::kLeftEighthBankToDiagUp25Seq3, Narrow::kLeftEighthBankToDiagUp25Seq4 }}, // LeftEighthBankToDiagUp25
        {{ Narrow::kRightEighthBankToDiagUp25Seq0, Narrow::kRightEighthBankToDiagUp25Seq1, Narrow::kRightEighthBankToDiagUp25Seq2, Narrow::kRightEighthBankToDiagUp25Seq3, Narrow::kRightEighthBankToDiagUp25Seq4 }}, // RightEighthBankToDiagUp25
        {{ Narrow::kLeftEighthBankToDiagDown25Seq0, Narrow::kLeftEighthBankToDiagDown25Seq1, Narrow::kLeftEighthBankToDiagDown25Seq2, Narrow::kLeftEighthBankToDiagDown25Seq3, Narrow::kLeftEighthBankToDiagDown25Seq4 }}, // LeftEighthBankToDiagDown25
        {{ Narrow::kRightEighthBankToDiagDown25Seq0, Narrow::kRightEighthBankToDiagDown25Seq1, Narrow::kRightEighthBankToDiagDown25Seq2, Narrow::kRightEighthBankToDiagDown25Seq3, Narrow::kRightEighthBankToDiagDown25Seq4 }}, // RightEighthBankToDiagDown25
        {{ Narrow::kLeftEighthBankToOrthogonalUp25Seq0, Narrow::kLeftEighthBankToOrthogonalUp25Seq1, Narrow::kLeftEighthBankToOrthogonalUp25Seq2, Narrow::kLeftEighthBankToOrthogonalUp25Seq3, Narrow::kLeftEighthBankToOrthogonalUp25Seq4 }}, // LeftEighthBankToOrthogonalUp25
        {{ Narrow::kRightEighthBankToOrthogonalUp25Seq0, Narrow::kRightEighthBankToOrthogonalUp25Seq1, Narrow::kRightEighthBankToOrthogonalUp25Seq2, Narrow::kRightEighthBankToOrthogonalUp25Seq3, Narrow::kRightEighthBankToOrthogonalUp25Seq4 }}, // RightEighthBankToOrthogonalUp25
        {{ Narrow::kLeftEighthBankToOrthogonalDown25Seq0, Narrow::kLeftEighthBankToOrthogonalDown25Seq1, Narrow::kLeftEighthBankToOrthogonalDown25Seq2, Narrow::kLeftEighthBankToOrthogonalDown25Seq3, Narrow::kLeftEighthBankToOrthogonalDown25Seq4 }}, // LeftEighthBankToOrthogonalDown25
        {{ Narrow::kRightEighthBankToOrthogonalDown25Seq0, Narrow::kRightEighthBankToOrthogonalDown25Seq1, Narrow::kRightEighthBankToOrthogonalDown25Seq2, Narrow::kRightEighthBankToOrthogonalDown25Seq3, Narrow::kRightEighthBankToOrthogonalDown25Seq4 }}, // RightEighthBankToOrthogonalDown25
        {{ Narrow::kDiagBrakesSeq0, Narrow::kDiagBrakesSeq1, Narrow::kDiagBrakesSeq2, Narrow::kDiagBrakesSeq3 }}, // DiagBrakes
        {{ Narrow::kDiagBlockBrakesSeq0, Narrow::kDiagBlockBrakesSeq1, Narrow::kDiagBlockBrakesSeq2, Narrow::kDiagBlockBrakesSeq3 }}, // DiagBlockBrakes
        {{ Narrow::kDown25BrakesSeq0 }}, // Down25Brakes
        {{ Narrow::kDiagBoosterSeq0, Narrow::kDiagBoosterSeq1, Narrow::kDiagBoosterSeq2, Narrow::kDiagBoosterSeq3 }}, // DiagBooster
        {{ Narrow::kDiagFlatToUp60LongBaseSeq0, Narrow::kDiagFlatToUp60LongBaseSeq1, Narrow::kDiagFlatToUp60LongBaseSeq2, Narrow::kDiagFlatToUp60LongBaseSeq3, Narrow::kDiagFlatToUp60LongBaseSeq4, Narrow::kDiagFlatToUp60LongBaseSeq5, Narrow::kDiagFlatToUp60LongBaseSeq6, Narrow::kDiagFlatToUp60LongBaseSeq7, Narrow::kDiagFlatToUp60LongBaseSeq8, Narrow::kDiagFlatToUp60LongBaseSeq9 }}, // DiagFlatToUp60LongBase
        {{ Narrow::kDiagUp60ToFlatLongBaseSeq0, Narrow::kDiagUp60ToFlatLongBaseSeq1, Narrow::kDiagUp60ToFlatLongBaseSeq2, Narrow::kDiagUp60ToFlatLongBaseSeq3, Narrow::kDiagUp60ToFlatLongBaseSeq4, Narrow::kDiagUp60ToFlatLongBaseSeq5, Narrow::kDiagUp60ToFlatLongBaseSeq6, Narrow::kDiagUp60ToFlatLongBaseSeq7, Narrow::kDiagUp60ToFlatLongBaseSeq8, Narrow::kDiagUp60ToFlatLongBaseSeq9 }}, // DiagUp60ToFlatLongBase
        {{ Narrow::kDiagFlatToDown60LongBaseSeq0, Narrow::kDiagFlatToDown60LongBaseSeq1, Narrow::kDiagFlatToDown60LongBaseSeq2, Narrow::kDiagFlatToDown60LongBaseSeq3, Narrow::kDiagFlatToDown60LongBaseSeq4, Narrow::kDiagFlatToDown60LongBaseSeq5, Narrow::kDiagFlatToDown60LongBaseSeq6, Narrow::kDiagFlatToDown60LongBaseSeq7, Narrow::kDiagFlatToDown60LongBaseSeq8, Narrow::kDiagFlatToDown60LongBaseSeq9 }}, // DiagFlatToDown60LongBase
        {{ Narrow::kDiagDown60ToFlatLongBaseSeq0, Narrow::kDiagDown60ToFlatLongBaseSeq1, Narrow::kDiagDown60ToFlatLongBaseSeq2, Narrow::kDiagDown60ToFlatLongBaseSeq3, Narrow::kDiagDown60ToFlatLongBaseSeq4, Narrow::kDiagDown60ToFlatLongBaseSeq5, Narrow::kDiagDown60ToFlatLongBaseSeq6, Narrow::kDiagDown60ToFlatLongBaseSeq7, Narrow::kDiagDown60ToFlatLongBaseSeq8, Narrow::kDiagDown60ToFlatLongBaseSeq9 }}, // DiagDown60ToFlatLongBase
        {{ Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq0, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq1, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq2, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq3, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq4, Narrow::kLeftEighthDiveLoopUpToOrthogonalSeq5 }}, // LeftEighthDiveLoopUpToOrthogonal
        {{ Narrow::kRightEighthDiveLoopUpToOrthogonalSeq0, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq1, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq2, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq3, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq4, Narrow::kRightEighthDiveLoopUpToOrthogonalSeq5 }}, // RightEighthDiveLoopUpToOrthogonal
        {{ Narrow::kLeftEighthDiveLoopDownToDiagSeq0, Narrow::kLeftEighthDiveLoopDownToDiagSeq1, Narrow::kLeftEighthDiveLoopDownToDiagSeq2, Narrow::kLeftEighthDiveLoopDownToDiagSeq3, Narrow::kLeftEighthDiveLoopDownToDiagSeq4, Narrow::kLeftEighthDiveLoopDownToDiagSeq5 }}, // LeftEighthDiveLoopDownToDiag
        {{ Narrow::kRightEighthDiveLoopDownToDiagSeq0, Narrow::kRightEighthDiveLoopDownToDiagSeq1, Narrow::kRightEighthDiveLoopDownToDiagSeq2, Narrow::kRightEighthDiveLoopDownToDiagSeq3, Narrow::kRightEighthDiveLoopDownToDiagSeq4, Narrow::kRightEighthDiveLoopDownToDiagSeq5 }}, // RightEighthDiveLoopDownToDiag
        {{ Narrow::kDiagDown25BrakesSeq0, Narrow::kDiagDown25BrakesSeq1, Narrow::kDiagDown25BrakesSeq2, Narrow::kDiagDown25BrakesSeq3 }}, // DiagDown25Brakes
    };

    // clang-format on

    static_assert(std::size(kBlockedSegmentsWideTrain) == EnumValue(TrackElemType::Count));

    static constexpr const std::array<uint16_t, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece>* kBlockedSegmentsMap[] = {
        kBlockedSegmentsNarrow,                 // narrow
        kBlockedSegmentsInverted,               // inverted
        kBlockedSegmentsWide,                   // wide
        kBlockedSegmentsSuspendedSwingingTrain, // suspendedSwinging
        kBlockedSegmentsWideTrain,              // wideTrain
    };

    static_assert(std::size(kBlockedSegmentsMap) == EnumValue(BlockedSegmentsType::count));

    void BlockSegmentsForTrackSequence(
        PaintSession& session, const uint8_t trackSequence, const Direction direction, const uint16_t height,
        const TrackElemType trackElemType, const BlockedSegmentsType blockedSegmentsType)
    {
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                kBlockedSegmentsMap[EnumValue(blockedSegmentsType)][EnumValue(trackElemType)][trackSequence], direction),
            0xFFFF, 0);
    }
} // namespace OpenRCT2::BlockedSegments
