/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../SpriteIds.h"
#include "../../../drawing/ImageId.hpp"
#include "../../../paint/Boundbox.h"
#include "../../../ride/RideData.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::MiniHelicopters
{
    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatSprites = {{
        16870,
        16871,
        16870,
        16871,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kFlatBoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    } };

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kStationBoundBoxes = { {
        { { 0, 6, 1 }, { 32, 20, 3 } },
        { { 6, 0, 1 }, { 20, 32, 3 } },
        { { 0, 6, 1 }, { 32, 20, 3 } },
        { { 6, 0, 1 }, { 20, 32, 3 } },
    } };

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25Sprites = {{
        16892,
        16893,
        16894,
        16895,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kUp25BoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    } };

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        16884,
        16885,
        16886,
        16887,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kFlatToUp25BoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    } };

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        16888,
        16889,
        16890,
        16891,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kUp25ToFlatBoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            16877,
            kImageIndexUndefined,
            16876,
            16875,
        }},
        {{
            16880,
            kImageIndexUndefined,
            16879,
            16878,
        }},
        {{
            16883,
            kImageIndexUndefined,
            16882,
            16881,
        }},
        {{
            16874,
            kImageIndexUndefined,
            16873,
            16872,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftQuarterTurn3TilesBoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 16, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        } },
        { {
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileSprites = {{
        16896,
        16897,
        16898,
        16899,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kLeftQuarterTurn1TileBoundBoxes = { {
        { { 6, 2, 0 }, { 26, 24, 1 } },
        { { 0, 0, 0 }, { 26, 26, 1 } },
        { { 2, 6, 0 }, { 24, 26, 1 } },
        { { 6, 6, 0 }, { 24, 24, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kSpinningTunnelSprites = {{
        {{ 28773, 16870, }},
        {{ 28774, 16871, }},
        {{ 28773, 16870, }},
        {{ 28774, 16871, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections> kSpinningTunnelOffsets = { {
        { { { 0, 6, -2 }, { 0, 0, 0 } } },
        { { { 6, 0, -2 }, { 0, 0, 0 } } },
        { { { 0, 6, -2 }, { 0, 0, 0 } } },
        { { { 6, 0, -2 }, { 0, 0, 0 } } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kSpinningTunnelBoundBoxes = { {
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 6, 0 }, { 32, 20, 3 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 6, 0, 0 }, { 20, 32, 3 } } } },
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 6, 0 }, { 32, 20, 3 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 6, 0, 0 }, { 20, 32, 3 } } } },
    } };

    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0], nullptr, &kFlatBoundBoxes[0] },             // Flat
        { &kFlatSprites[0], nullptr, &kStationBoundBoxes[0] },          // EndStation
        { &kFlatSprites[0], nullptr, &kStationBoundBoxes[0] },          // BeginStation
        { &kFlatSprites[0], nullptr, &kStationBoundBoxes[0] },          // MiddleStation
        { &kUp25Sprites[0], nullptr, &kUp25BoundBoxes[0] },             // Up25
        { nullptr, nullptr, nullptr },                                  // Up60
        { &kFlatToUp25Sprites[0], nullptr, &kFlatToUp25BoundBoxes[0] }, // FlatToUp25
        { nullptr, nullptr, nullptr },                                  // Up25ToUp60
        { nullptr, nullptr, nullptr },                                  // Up60ToUp25
        { &kUp25ToFlatSprites[0], nullptr, &kUp25ToFlatBoundBoxes[0] }, // Up25ToFlat
        { nullptr, nullptr, nullptr },                                  // Down25
        { nullptr, nullptr, nullptr },                                  // Down60
        { nullptr, nullptr, nullptr },                                  // FlatToDown25
        { nullptr, nullptr, nullptr },                                  // Down25ToDown60
        { nullptr, nullptr, nullptr },                                  // Down60ToDown25
        { nullptr, nullptr, nullptr },                                  // Down25ToFlat
        { nullptr, nullptr, nullptr },                                  // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr },                                  // RightQuarterTurn5Tiles
        { nullptr, nullptr, nullptr },                                  // FlatToLeftBank
        { nullptr, nullptr, nullptr },                                  // FlatToRightBank
        { nullptr, nullptr, nullptr },                                  // LeftBankToFlat
        { nullptr, nullptr, nullptr },                                  // RightBankToFlat
        { nullptr, nullptr, nullptr },                                  // BankedLeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr },                                  // BankedRightQuarterTurn5Tiles
        { nullptr, nullptr, nullptr },                                  // LeftBankToUp25
        { nullptr, nullptr, nullptr },                                  // RightBankToUp25
        { nullptr, nullptr, nullptr },                                  // Up25ToLeftBank
        { nullptr, nullptr, nullptr },                                  // Up25ToRightBank
        { nullptr, nullptr, nullptr },                                  // LeftBankToDown25
        { nullptr, nullptr, nullptr },                                  // RightBankToDown25
        { nullptr, nullptr, nullptr },                                  // Down25ToLeftBank
        { nullptr, nullptr, nullptr },                                  // Down25ToRightBank
        { nullptr, nullptr, nullptr },                                  // LeftBank
        { nullptr, nullptr, nullptr },                                  // RightBank
        { nullptr, nullptr, nullptr },                                  // LeftQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr },                                  // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr },                                  // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr },                                  // RightQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr },                                  // SBendLeft
        { nullptr, nullptr, nullptr },                                  // SBendRight
        { nullptr, nullptr, nullptr },                                  // LeftVerticalLoop
        { nullptr, nullptr, nullptr },                                  // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], nullptr, &kLeftQuarterTurn3TilesBoundBoxes[0][0] }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr },                                                              // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr },                                                      // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr },                                                      // RightBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn3TilesDown25
        { &kLeftQuarterTurn1TileSprites[0], nullptr, &kLeftQuarterTurn1TileBoundBoxes[0] }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr },                                                      // LeftTwistDownToUp
        { nullptr, nullptr, nullptr },                                                      // RightTwistDownToUp
        { nullptr, nullptr, nullptr },                                                      // LeftTwistUpToDown
        { nullptr, nullptr, nullptr },                                                      // RightTwistUpToDown
        { nullptr, nullptr, nullptr },                                                      // HalfLoopUp
        { nullptr, nullptr, nullptr },                                                      // HalfLoopDown
        { nullptr, nullptr, nullptr },                                                      // LeftCorkscrewUp
        { nullptr, nullptr, nullptr },                                                      // RightCorkscrewUp
        { nullptr, nullptr, nullptr },                                                      // LeftCorkscrewDown
        { nullptr, nullptr, nullptr },                                                      // RightCorkscrewDown
        { nullptr, nullptr, nullptr },                                                      // FlatToUp60
        { nullptr, nullptr, nullptr },                                                      // Up60ToFlat
        { nullptr, nullptr, nullptr },                                                      // FlatToDown60
        { nullptr, nullptr, nullptr },                                                      // Down60ToFlat
        { nullptr, nullptr, nullptr },                                                      // TowerBase
        { nullptr, nullptr, nullptr },                                                      // TowerSection
        { nullptr, nullptr, nullptr },                                                      // FlatCovered
        { nullptr, nullptr, nullptr },                                                      // Up25Covered
        { nullptr, nullptr, nullptr },                                                      // Up60Covered
        { nullptr, nullptr, nullptr },                                                      // FlatToUp25Covered
        { nullptr, nullptr, nullptr },                                                      // Up25ToUp60Covered
        { nullptr, nullptr, nullptr },                                                      // Up60ToUp25Covered
        { nullptr, nullptr, nullptr },                                                      // Up25ToFlatCovered
        { nullptr, nullptr, nullptr },                                                      // Down25Covered
        { nullptr, nullptr, nullptr },                                                      // Down60Covered
        { nullptr, nullptr, nullptr },                                                      // FlatToDown25Covered
        { nullptr, nullptr, nullptr },                                                      // Down25ToDown60Covered
        { nullptr, nullptr, nullptr },                                                      // Down60ToDown25Covered
        { nullptr, nullptr, nullptr },                                                      // Down25ToFlatCovered
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr },                                                      // SBendLeftCovered
        { nullptr, nullptr, nullptr },                                                      // SBendRightCovered
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterTurn3TilesCovered
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn3TilesCovered
        { nullptr, nullptr, nullptr },                                                      // LeftHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr },                                                      // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr },                                                      // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr },                                                      // RightHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr },                                                      // LeftHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr },                                                      // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr },                                                      // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr },                                                      // RightHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr },                                                      // RightQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr },                                                      // Brakes
        { nullptr, nullptr, nullptr },                                                      // Booster
        { nullptr, nullptr, nullptr },                                                      // Maze
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr },                                                      // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr },                                                      // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr },                                                      // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr },                                                      // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr },                                                      // RightQuarterHelixLargeDown
        { nullptr, nullptr, nullptr },                                                      // Up25LeftBanked
        { nullptr, nullptr, nullptr },                                                      // Up25RightBanked
        { nullptr, nullptr, nullptr },                                                      // Waterfall
        { nullptr, nullptr, nullptr },                                                      // Rapids
        { nullptr, nullptr, nullptr },                                                      // OnRidePhoto
        { nullptr, nullptr, nullptr },                                                      // Down25LeftBanked
        { nullptr, nullptr, nullptr },                                                      // Down25RightBanked
        { nullptr, nullptr, nullptr },                                                      // Watersplash
        { nullptr, nullptr, nullptr },                                                      // FlatToUp60LongBase
        { nullptr, nullptr, nullptr },                                                      // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr },                                                      // Whirlpool
        { nullptr, nullptr, nullptr },                                                      // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr },                                                      // FlatToDown60LongBase
        { nullptr, nullptr, nullptr },                                                      // CableLiftHill
        { nullptr, nullptr, nullptr },                                                      // ReverseFreefallSlope
        { nullptr, nullptr, nullptr },                                                      // ReverseFreefallVertical
        { nullptr, nullptr, nullptr },                                                      // Up90
        { nullptr, nullptr, nullptr },                                                      // Down90
        { nullptr, nullptr, nullptr },                                                      // Up60ToUp90
        { nullptr, nullptr, nullptr },                                                      // Down90ToDown60
        { nullptr, nullptr, nullptr },                                                      // Up90ToUp60
        { nullptr, nullptr, nullptr },                                                      // Down60ToDown90
        { nullptr, nullptr, nullptr },                                                      // BrakeForDrop
        { nullptr, nullptr, nullptr },                                                      // LeftEighthToDiag
        { nullptr, nullptr, nullptr },                                                      // RightEighthToDiag
        { nullptr, nullptr, nullptr },                                                      // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr },                                                      // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr },                                                      // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr },                                                      // RightEighthBankToDiag
        { nullptr, nullptr, nullptr },                                                      // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr },                                                      // RightEighthBankToOrthogonal
        { nullptr, nullptr, nullptr },                                                      // DiagFlat
        { nullptr, nullptr, nullptr },                                                      // DiagUp25
        { nullptr, nullptr, nullptr },                                                      // DiagUp60
        { nullptr, nullptr, nullptr },                                                      // DiagFlatToUp25
        { nullptr, nullptr, nullptr },                                                      // DiagUp25ToUp60
        { nullptr, nullptr, nullptr },                                                      // DiagUp60ToUp25
        { nullptr, nullptr, nullptr },                                                      // DiagUp25ToFlat
        { nullptr, nullptr, nullptr },                                                      // DiagDown25
        { nullptr, nullptr, nullptr },                                                      // DiagDown60
        { nullptr, nullptr, nullptr },                                                      // DiagFlatToDown25
        { nullptr, nullptr, nullptr },                                                      // DiagDown25ToDown60
        { nullptr, nullptr, nullptr },                                                      // DiagDown60ToDown25
        { nullptr, nullptr, nullptr },                                                      // DiagDown25ToFlat
        { nullptr, nullptr, nullptr },                                                      // DiagFlatToUp60
        { nullptr, nullptr, nullptr },                                                      // DiagUp60ToFlat
        { nullptr, nullptr, nullptr },                                                      // DiagFlatToDown60
        { nullptr, nullptr, nullptr },                                                      // DiagDown60ToFlat
        { nullptr, nullptr, nullptr },                                                      // DiagFlatToLeftBank
        { nullptr, nullptr, nullptr },                                                      // DiagFlatToRightBank
        { nullptr, nullptr, nullptr },                                                      // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr },                                                      // DiagRightBankToFlat
        { nullptr, nullptr, nullptr },                                                      // DiagLeftBankToUp25
        { nullptr, nullptr, nullptr },                                                      // DiagRightBankToUp25
        { nullptr, nullptr, nullptr },                                                      // DiagUp25ToLeftBank
        { nullptr, nullptr, nullptr },                                                      // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr },                                                      // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr },                                                      // DiagRightBankToDown25
        { nullptr, nullptr, nullptr },                                                      // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr },                                                      // DiagDown25ToRightBank
        { nullptr, nullptr, nullptr },                                                      // DiagLeftBank
        { nullptr, nullptr, nullptr },                                                      // DiagRightBank
        { nullptr, nullptr, nullptr },                                                      // LogFlumeReverser
        { &kSpinningTunnelSprites[0][0], &kSpinningTunnelOffsets[0][0], &kSpinningTunnelBoundBoxes[0][0] }, // SpinningTunnel
        { nullptr, nullptr, nullptr }, // LeftBarrelRollUpToDown
        { nullptr, nullptr, nullptr }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr }, // RightBarrelRollDownToUp
        { nullptr, nullptr, nullptr }, // LeftBankToLeftQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr }, // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr }, // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr }, // PoweredLift
        { nullptr, nullptr, nullptr }, // LeftLargeHalfLoopUp
        { nullptr, nullptr, nullptr }, // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr }, // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr }, // RightLargeHalfLoopDown
        { nullptr, nullptr, nullptr }, // LeftFlyerTwistUp
        { nullptr, nullptr, nullptr }, // RightFlyerTwistUp
        { nullptr, nullptr, nullptr }, // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr }, // RightFlyerTwistDown
        { nullptr, nullptr, nullptr }, // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr }, // FlyerHalfLoopInvertedDown
        { nullptr, nullptr, nullptr }, // LeftFlyerCorkscrewUp
        { nullptr, nullptr, nullptr }, // RightFlyerCorkscrewUp
        { nullptr, nullptr, nullptr }, // LeftFlyerCorkscrewDown
        { nullptr, nullptr, nullptr }, // RightFlyerCorkscrewDown
        { nullptr, nullptr, nullptr }, // HeartLineTransferUp
        { nullptr, nullptr, nullptr }, // HeartLineTransferDown
        { nullptr, nullptr, nullptr }, // LeftHeartLineRoll
        { nullptr, nullptr, nullptr }, // RightHeartLineRoll
        { nullptr, nullptr, nullptr }, // MinigolfHoleA
        { nullptr, nullptr, nullptr }, // MinigolfHoleB
        { nullptr, nullptr, nullptr }, // MinigolfHoleC
        { nullptr, nullptr, nullptr }, // MinigolfHoleD
        { nullptr, nullptr, nullptr }, // MinigolfHoleE
        { nullptr, nullptr, nullptr }, // MultiDimInvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr }, // Up90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr }, // LeftReverser
        { nullptr, nullptr, nullptr }, // RightReverser
        { nullptr, nullptr, nullptr }, // AirThrustTopCap
        { nullptr, nullptr, nullptr }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr }, // AirThrustVerticalDownToLevel
        { nullptr, nullptr, nullptr }, // BlockBrakes
        { nullptr, nullptr, nullptr }, // LeftBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr }, // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr }, // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr }, // RightBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr }, // LeftBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr }, // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr }, // RightBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr }, // Up25ToLeftBankedUp25
        { nullptr, nullptr, nullptr }, // Up25ToRightBankedUp25
        { nullptr, nullptr, nullptr }, // LeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr }, // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr }, // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr }, // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr }, // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr }, // RightBankedDown25ToDown25
        { nullptr, nullptr, nullptr }, // LeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr }, // RightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr }, // LeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr }, // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr }, // RightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr }, // FlatToLeftBankedUp25
        { nullptr, nullptr, nullptr }, // FlatToRightBankedUp25
        { nullptr, nullptr, nullptr }, // LeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr }, // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr }, // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr }, // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr }, // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr }, // RightBankedDown25ToFlat
        { nullptr, nullptr, nullptr }, // LeftQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr }, // RightQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr }, // LeftQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr }, // RightQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr }, // MultiDimUp90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr }, // MultiDimFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr }, // MultiDimInvertedUp90ToFlatQuarterLoop
        { nullptr, nullptr, nullptr }, // RotationControlToggle
        { nullptr, nullptr, nullptr }, // FlatTrack1x4A
        { nullptr, nullptr, nullptr }, // FlatTrack2x2
        { nullptr, nullptr, nullptr }, // FlatTrack4x4
        { nullptr, nullptr, nullptr }, // FlatTrack2x4
        { nullptr, nullptr, nullptr }, // FlatTrack1x5
        { nullptr, nullptr, nullptr }, // FlatTrack1x1A
        { nullptr, nullptr, nullptr }, // FlatTrack1x4B
        { nullptr, nullptr, nullptr }, // FlatTrack1x1B
        { nullptr, nullptr, nullptr }, // FlatTrack1x4C
        { nullptr, nullptr, nullptr }, // FlatTrack3x3
        { nullptr, nullptr, nullptr }, // LeftLargeCorkscrewUp
        { nullptr, nullptr, nullptr }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr }, // RightLargeCorkscrewDown
        { nullptr, nullptr, nullptr }, // LeftMediumHalfLoopUp
        { nullptr, nullptr, nullptr }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr }, // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr }, // RightMediumHalfLoopDown
        { nullptr, nullptr, nullptr }, // LeftZeroGRollUp
        { nullptr, nullptr, nullptr }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr }, // LeftZeroGRollDown
        { nullptr, nullptr, nullptr }, // RightZeroGRollDown
        { nullptr, nullptr, nullptr }, // LeftLargeZeroGRollUp
        { nullptr, nullptr, nullptr }, // RightLargeZeroGRollUp
        { nullptr, nullptr, nullptr }, // LeftLargeZeroGRollDown
        { nullptr, nullptr, nullptr }, // RightLargeZeroGRollDown
        { nullptr, nullptr, nullptr }, // LeftFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr }, // RightFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr }, // LeftFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr }, // RightFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr }, // LeftFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr }, // RightFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr }, // LeftFlyerLargeHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr }, // RightFlyerLargeHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr }, // FlyerHalfLoopInvertedUp
        { nullptr, nullptr, nullptr }, // FlyerHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr }, // LeftEighthToDiagUp25
        { nullptr, nullptr, nullptr }, // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr }, // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr }, // RightEighthToDiagDown25
        { nullptr, nullptr, nullptr }, // LeftEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr }, // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr }, // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr }, // RightEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr }, // DiagUp25ToLeftBankedUp25
        { nullptr, nullptr, nullptr }, // DiagUp25ToRightBankedUp25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr }, // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr }, // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr }, // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr }, // DiagRightBankedDown25ToDown25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr }, // DiagRightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr }, // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr }, // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr }, // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr }, // DiagRightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr }, // DiagFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr }, // DiagFlatToRightBankedUp25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr }, // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr }, // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr }, // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr }, // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr }, // DiagRightBankedDown25ToFlat
        { nullptr, nullptr, nullptr }, // DiagUp25LeftBanked
        { nullptr, nullptr, nullptr }, // DiagUp25RightBanked
        { nullptr, nullptr, nullptr }, // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr }, // DiagDown25RightBanked
        { nullptr, nullptr, nullptr }, // LeftEighthBankToDiagUp25
        { nullptr, nullptr, nullptr }, // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr }, // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr }, // RightEighthBankToDiagDown25
        { nullptr, nullptr, nullptr }, // LeftEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr }, // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr }, // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr }, // RightEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr }, // DiagBrakes
        { nullptr, nullptr, nullptr }, // DiagBlockBrakes
        { nullptr, nullptr, nullptr }, // Down25Brakes
        { nullptr, nullptr, nullptr }, // DiagBooster
        { nullptr, nullptr, nullptr }, // DiagFlatToUp60LongBase
        { nullptr, nullptr, nullptr }, // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr }, // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr }, // DiagDown60ToFlatLongBase
        { nullptr, nullptr, nullptr }, // LeftEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr }, // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr }, // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr }, // RightEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr }, // DiagDown25Brakes
    } };
} // namespace OpenRCT2::MiniHelicopters
