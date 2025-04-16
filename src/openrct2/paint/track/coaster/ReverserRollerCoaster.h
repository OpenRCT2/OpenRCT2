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

namespace OpenRCT2::ReverserRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            21504,
            21505,
            21504,
            21505,
        }},
        {{
            21520,
            21521,
            21520,
            21521,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        { { 0, 2, 1 }, { 32, 27, 2 } },
        { { 2, 0, 1 }, { 27, 32, 2 } },
        { { 0, 2, 1 }, { 32, 27, 2 } },
        { { 2, 0, 1 }, { 27, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            21516,
            21517,
            21518,
            21519,
        }},
        {{
            21530,
            21531,
            21532,
            21533,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            21508,
            21509,
            21510,
            21511,
        }},
        {{
            21522,
            21523,
            21524,
            21525,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            21512,
            21513,
            21514,
            21515,
        }},
        {{
            21526,
            21527,
            21528,
            21529,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            21543,
            kImageIndexUndefined,
            21542,
            21541,
            kImageIndexUndefined,
            21540,
            21539,
        }},
        {{
            21548,
            kImageIndexUndefined,
            21547,
            21546,
            kImageIndexUndefined,
            21545,
            21544,
        }},
        {{
            21553,
            kImageIndexUndefined,
            21552,
            21551,
            kImageIndexUndefined,
            21550,
            21549,
        }},
        {{
            21538,
            kImageIndexUndefined,
            21537,
            21536,
            kImageIndexUndefined,
            21535,
            21534,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 2, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 2, 0, 0 }, 
        }},
        {{
            { 2, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 2, 0 }, 
        }},
        {{
            { 0, 2, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 2, 0, 0 }, 
        }},
        {{
            { 2, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 2, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 2, 0 }, { 32, 27, 2 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 2 } },
            { { 0, 16, 0 }, { 16, 16, 2 } },
            {},
            { { 16, 0, 0 }, { 16, 34, 2 } },
            { { 2, 0, 0 }, { 32, 32, 2 } },
        }},
        {{
            { { 2, 0, 0 }, { 27, 32, 2 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 2 } },
            { { 16, 16, 0 }, { 16, 16, 2 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
        }},
        {{
            { { 0, 2, 0 }, { 32, 32, 2 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 2 } },
            { { 16, 0, 0 }, { 16, 16, 2 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
        }},
        {{
            { { 2, 0, 0 }, { 32, 32, 2 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 2 } },
            { { 0, 0, 0 }, { 16, 16, 2 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 2 } },
            { { 0, 2, 0 }, { 32, 32, 2 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            21566,
            21567,
            21568,
            21569,
        }},
        {{
            21570,
            21571,
            21572,
            21573,
        }},
        {{
            21569,
            21568,
            21567,
            21566,
        }},
        {{
            21573,
            21572,
            21571,
            21570,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 0, 0 }, { 32, 26, 2 } },
            { { 0, 6, 0 }, { 32, 26, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
        }},
        {{
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 0, 0, 0 }, { 26, 32, 2 } },
            { { 6, 0, 0 }, { 26, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
        }},
        {{
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 6, 0 }, { 32, 26, 2 } },
            { { 0, 0, 0 }, { 32, 26, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
        }},
        {{
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 6, 0, 0 }, { 26, 32, 2 } },
            { { 0, 0, 0 }, { 26, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            21574,
            21575,
            21576,
            21577,
        }},
        {{
            21578,
            21579,
            21580,
            21581,
        }},
        {{
            21577,
            21576,
            21575,
            21574,
        }},
        {{
            21581,
            21580,
            21579,
            21578,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            21559,
            kImageIndexUndefined,
            21558,
            21557,
        }},
        {{
            21562,
            kImageIndexUndefined,
            21561,
            21560,
        }},
        {{
            21565,
            kImageIndexUndefined,
            21564,
            21563,
        }},
        {{
            21556,
            kImageIndexUndefined,
            21555,
            21554,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 2 } },
            {},
            { { 16, 0, 0 }, { 16, 16, 2 } },
            { { 6, 0, 0 }, { 20, 32, 2 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 2 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 2 } },
            { { 0, 6, 0 }, { 32, 20, 2 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 2 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 2 } },
            { { 6, 0, 0 }, { 20, 32, 2 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 2 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 2 } },
            { { 0, 6, 0 }, { 32, 20, 2 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        21506,
        21507,
        21506,
        21507,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kLeftReverserSprites = {{
        {{
            21582,
            21585,
            21586,
            21587,
            21583,
            21584,
        }},
        {{
            21588,
            21591,
            21592,
            21593,
            21589,
            21590,
        }},
        {{
            21594,
            21597,
            21598,
            21599,
            21595,
            21596,
        }},
        {{
            21600,
            21603,
            21604,
            21605,
            21601,
            21602,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kLeftReverserBoundBoxes = {{
        {{
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
        }},
        {{
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
        }},
        {{
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
            { { 0, 2, 0 }, { 32, 27, 2 } },
        }},
        {{
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
            { { 2, 0, 0 }, { 27, 32, 2 } },
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false }, // Flat
        { &kBrakesSprites[0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
        { &kBrakesSprites[0], nullptr, &kStationBoundBoxes[0], false }, // BeginStation
        { &kBrakesSprites[0], nullptr, &kStationBoundBoxes[0], false }, // MiddleStation
        { &kUp25Sprites[0][0], nullptr, &kUp25BoundBoxes[0], false }, // Up25
        { nullptr, nullptr, nullptr, false }, // Up60
        { &kFlatToUp25Sprites[0][0], nullptr, &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { nullptr, nullptr, nullptr, false }, // Up25ToUp60
        { nullptr, nullptr, nullptr, false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], nullptr, &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0], &kLeftQuarterTurn5TilesOffsets[0][0], &kLeftQuarterTurn5TilesBoundBoxes[0][0], false }, // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, false }, // FlatToLeftBank
        { nullptr, nullptr, nullptr, false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { nullptr, nullptr, nullptr, false }, // BankedLeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // BankedRightQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, false }, // LeftBankToUp25
        { nullptr, nullptr, nullptr, false }, // RightBankToUp25
        { nullptr, nullptr, nullptr, false }, // Up25ToLeftBank
        { nullptr, nullptr, nullptr, false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBank
        { nullptr, nullptr, nullptr, false }, // LeftBank
        { nullptr, nullptr, nullptr, true }, // RightBank
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], nullptr, &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], nullptr, &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], nullptr, &kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, false }, // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false }, // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true }, // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true }, // RightTwistUpToDown
        { nullptr, nullptr, nullptr, false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true }, // HalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftCorkscrewUp
        { nullptr, nullptr, nullptr, false }, // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // FlatToUp60
        { nullptr, nullptr, nullptr, false }, // Up60ToFlat
        { nullptr, nullptr, nullptr, true }, // FlatToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToFlat
        { nullptr, nullptr, nullptr, false }, // TowerBase
        { nullptr, nullptr, nullptr, false }, // TowerSection
        { nullptr, nullptr, nullptr, false }, // FlatCovered
        { nullptr, nullptr, nullptr, false }, // Up25Covered
        { nullptr, nullptr, nullptr, false }, // Up60Covered
        { nullptr, nullptr, nullptr, false }, // FlatToUp25Covered
        { nullptr, nullptr, nullptr, false }, // Up25ToUp60Covered
        { nullptr, nullptr, nullptr, false }, // Up60ToUp25Covered
        { nullptr, nullptr, nullptr, false }, // Up25ToFlatCovered
        { nullptr, nullptr, nullptr, false }, // Down25Covered
        { nullptr, nullptr, nullptr, false }, // Down60Covered
        { nullptr, nullptr, nullptr, false }, // FlatToDown25Covered
        { nullptr, nullptr, nullptr, false }, // Down25ToDown60Covered
        { nullptr, nullptr, nullptr, false }, // Down60ToDown25Covered
        { nullptr, nullptr, nullptr, false }, // Down25ToFlatCovered
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr, false }, // SBendLeftCovered
        { nullptr, nullptr, nullptr, false }, // SBendRightCovered
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn3TilesCovered
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn3TilesCovered
        { nullptr, nullptr, nullptr, false }, // LeftHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, false }, // LeftHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, false }, // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0], nullptr, &kBrakesBoundBoxes[0], false }, // Brakes
        { nullptr, nullptr, nullptr, false }, // Booster
        { nullptr, nullptr, nullptr, false }, // Maze
        { nullptr, nullptr, nullptr, false }, // LeftQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, false }, // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, true }, // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, true }, // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, false }, // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, false }, // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, true }, // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, true }, // RightQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, false }, // Up25LeftBanked
        { nullptr, nullptr, nullptr, false }, // Up25RightBanked
        { nullptr, nullptr, nullptr, false }, // Waterfall
        { nullptr, nullptr, nullptr, false }, // Rapids
        { nullptr, nullptr, nullptr, false }, // OnRidePhoto
        { nullptr, nullptr, nullptr, true }, // Down25LeftBanked
        { nullptr, nullptr, nullptr, true }, // Down25RightBanked
        { nullptr, nullptr, nullptr, false }, // Watersplash
        { nullptr, nullptr, nullptr, false }, // FlatToUp60LongBase
        { nullptr, nullptr, nullptr, false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // Whirlpool
        { nullptr, nullptr, nullptr, true }, // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false }, // CableLiftHill
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallVertical
        { nullptr, nullptr, nullptr, false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { nullptr, nullptr, nullptr, false }, // Up60ToUp90
        { nullptr, nullptr, nullptr, true }, // Down90ToDown60
        { nullptr, nullptr, nullptr, false }, // Up90ToUp60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { nullptr, nullptr, nullptr, false }, // LeftEighthToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, false }, // DiagFlat
        { nullptr, nullptr, nullptr, false }, // DiagUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp60
        { nullptr, nullptr, nullptr, false }, // DiagFlatToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToUp60
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown60
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagFlatToUp60
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagFlatToLeftBank
        { nullptr, nullptr, nullptr, false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToFlat
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankToUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToLeftBank
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBank
        { nullptr, nullptr, nullptr, false }, // DiagLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagRightBank
        { nullptr, nullptr, nullptr, false }, // LogFlumeReverser
        { nullptr, nullptr, nullptr, false }, // SpinningTunnel
        { nullptr, nullptr, nullptr, false }, // LeftBarrelRollUpToDown
        { nullptr, nullptr, nullptr, false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true }, // RightBarrelRollDownToUp
        { nullptr, nullptr, nullptr, false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false }, // PoweredLift
        { nullptr, nullptr, nullptr, false }, // LeftLargeHalfLoopUp
        { nullptr, nullptr, nullptr, false }, // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightLargeHalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerTwistUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, true }, // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, true }, // RightFlyerTwistDown
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, true }, // LeftFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // RightFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, false }, // LeftFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // RightFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // HeartLineTransferUp
        { nullptr, nullptr, nullptr, false }, // HeartLineTransferDown
        { nullptr, nullptr, nullptr, false }, // LeftHeartLineRoll
        { nullptr, nullptr, nullptr, false }, // RightHeartLineRoll
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleA
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleB
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleC
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleD
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleE
        { nullptr, nullptr, nullptr, false }, // MultiDimInvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // Up90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr, false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { &kLeftReverserSprites[0][0], nullptr, &kLeftReverserBoundBoxes[0][0], false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { nullptr, nullptr, nullptr, false }, // BlockBrakes
        { nullptr, nullptr, nullptr, false }, // LeftBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, false }, // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, false }, // LeftBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, false }, // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, false }, // Up25ToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // Up25ToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // LeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr, false }, // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToDown25
        { nullptr, nullptr, nullptr, false }, // LeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // RightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // LeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, false }, // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr, false }, // FlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // FlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // LeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr, false }, // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr, false }, // MultiDimUp90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr, false }, // MultiDimFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // MultiDimInvertedUp90ToFlatQuarterLoop
        { nullptr, nullptr, nullptr, false }, // RotationControlToggle
        { nullptr, nullptr, nullptr, false }, // FlatTrack1x4A
        { nullptr, nullptr, nullptr, false }, // FlatTrack2x2
        { nullptr, nullptr, nullptr, false }, // FlatTrack4x4
        { nullptr, nullptr, nullptr, false }, // FlatTrack2x4
        { nullptr, nullptr, nullptr, false }, // FlatTrack1x5
        { nullptr, nullptr, nullptr, false }, // FlatTrack1x1A
        { nullptr, nullptr, nullptr, false }, // FlatTrack1x4B
        { nullptr, nullptr, nullptr, false }, // FlatTrack1x1B
        { nullptr, nullptr, nullptr, false }, // FlatTrack1x4C
        { nullptr, nullptr, nullptr, false }, // FlatTrack3x3
        { nullptr, nullptr, nullptr, false }, // LeftLargeCorkscrewUp
        { nullptr, nullptr, nullptr, false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // LeftMediumHalfLoopUp
        { nullptr, nullptr, nullptr, false }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightMediumHalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftZeroGRollUp
        { nullptr, nullptr, nullptr, false }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true }, // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true }, // RightZeroGRollDown
        { nullptr, nullptr, nullptr, false }, // LeftLargeZeroGRollUp
        { nullptr, nullptr, nullptr, false }, // RightLargeZeroGRollUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeZeroGRollDown
        { nullptr, nullptr, nullptr, true }, // RightLargeZeroGRollDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // LeftFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false }, // RightFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false }, // LeftFlyerLargeHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr, false }, // RightFlyerLargeHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr, false }, // LeftEighthToDiagUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToDiagDown25
        { nullptr, nullptr, nullptr, false }, // LeftEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToDown25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr, false }, // DiagFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagUp25LeftBanked
        { nullptr, nullptr, nullptr, false }, // DiagUp25RightBanked
        { nullptr, nullptr, nullptr, true }, // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr, true }, // DiagDown25RightBanked
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, false }, // DiagBrakes
        { nullptr, nullptr, nullptr, false }, // DiagBlockBrakes
        { nullptr, nullptr, nullptr, false }, // Down25Brakes
        { nullptr, nullptr, nullptr, false }, // DiagBooster
        { nullptr, nullptr, nullptr, false }, // DiagFlatToUp60LongBase
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // LeftEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, false }, // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, true }, // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, true }, // RightEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, false }, // DiagDown25Brakes
    } };
    // clang-format on
} // namespace OpenRCT2::ReverserRollerCoaster
