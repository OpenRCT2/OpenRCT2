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
#include "../../../paint/track/coaster/LatticeTriangle.h"
#include "../../../ride/RideData.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::LatticeTriangleAlt
{
    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>, 2>
    kEndStationSprites = {{
        {{
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NE_SW,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NW_SE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SW_NE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SE_NW,
            }},
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NE_SW,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NW_SE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SW_NE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SE_NW,
            }},
        }},
        {{
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW,
            }},
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE,
                SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW,
            }},
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_NE_SW,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_NW_SE,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_SW_NE,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DRIVE_TYRE_SE_NW,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW,
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBrakesSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_NE_SW + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_NW_SE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_SW_NE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_OPEN_SE_NW + 0,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_NE_SW + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_NW_SE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_SW_NE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BRAKE_ALT_CLOSED_SE_NW + 0,
        }},
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBoosterSprites = {{
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NE_SW + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_NW_SE + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SW_NE + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_ALT_SE_NW + 0,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBoosterBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kPoweredLiftSprites = {{
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_ALT_1 + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_ALT_2 + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_ALT_3 + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_ALT_4 + 0,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kPoweredLiftBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NE_SW + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_NW_SE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SW_NE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_OPEN_SE_NW + 0,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NE_SW + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_NW_SE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SW_NE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_BLOCK_BRAKE_ALT_CLOSED_SE_NW + 0,
        }},
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagBrakesSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_1 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_2 + 0,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_3 + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_OPEN_ALT_4 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_1 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_2 + 0,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_3 + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKE_CLOSED_ALT_4 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesOffsets = {{
        {{
            {},
            { -16, -16, 0 },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 0 },
        }},
        {{
            {},
            {},
            { -16, -16, 0 },
            {},
        }},
        {{
            { -16, -16, 0 },
            {},
            {},
            {},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagBlockBrakesSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_1 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_2 + 0,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_3 + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_OPEN_ALT_4 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_1 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_2 + 0,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_3 + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BLOCK_BRAKE_CLOSED_ALT_4 + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesOffsets = {{
        {{
            {},
            { -16, -16, 0 },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 0 },
        }},
        {{
            {},
            {},
            { -16, -16, 0 },
            {},
        }},
        {{
            { -16, -16, 0 },
            {},
            {},
            {},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBoosterSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_1 + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_2 + 0,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_3 + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_ALT_4 + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBoosterOffsets = {{
        {{
            {},
            { -16, -16, 0 },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 0 },
        }},
        {{
            {},
            {},
            { -16, -16, 0 },
            {},
        }},
        {{
            { -16, -16, 0 },
            {},
            {},
            {},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBoosterBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};

    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &OpenRCT2::LatticeTriangle::kFlatSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatBoundBoxes[0], false }, // Flat
        { &kEndStationSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kStationBoundBoxes[0], false }, // EndStation
        { &kStationSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kStationBoundBoxes[0], false }, // BeginStation
        { &kStationSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kStationBoundBoxes[0], false }, // MiddleStation
        { &OpenRCT2::LatticeTriangle::kUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25BoundBoxes[0], false }, // Up25
        { &OpenRCT2::LatticeTriangle::kUp60Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp60BoundBoxes[0], false }, // Up60
        { &OpenRCT2::LatticeTriangle::kFlatToUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &OpenRCT2::LatticeTriangle::kUp25ToUp60Sprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &OpenRCT2::LatticeTriangle::kUp60ToUp25Sprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &OpenRCT2::LatticeTriangle::kUp25ToFlatSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &OpenRCT2::LatticeTriangle::kLeftQuarterTurn5TilesSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftQuarterTurn5TilesBoundBoxes[0][0], false }, // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5Tiles
        { &OpenRCT2::LatticeTriangle::kFlatToLeftBankSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToLeftBankBoundBoxes[0][0], false }, // FlatToLeftBank
        { &OpenRCT2::LatticeTriangle::kFlatToRightBankSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { &OpenRCT2::LatticeTriangle::kBankedLeftQuarterTurn5TilesSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kBankedLeftQuarterTurn5TilesBoundBoxes[0][0][0], false }, // BankedLeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // BankedRightQuarterTurn5Tiles
        { &OpenRCT2::LatticeTriangle::kLeftBankToUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankToUp25BoundBoxes[0][0], false }, // LeftBankToUp25
        { &OpenRCT2::LatticeTriangle::kRightBankToUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightBankToUp25BoundBoxes[0][0], false }, // RightBankToUp25
        { &OpenRCT2::LatticeTriangle::kUp25ToLeftBankSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25ToLeftBankBoundBoxes[0][0], false }, // Up25ToLeftBank
        { &OpenRCT2::LatticeTriangle::kUp25ToRightBankSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25ToRightBankBoundBoxes[0][0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBank
        { &OpenRCT2::LatticeTriangle::kLeftBankSprites[0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankBoundBoxes[0], false }, // LeftBank
        { nullptr, nullptr, nullptr, true }, // RightBank
        { &OpenRCT2::LatticeTriangle::kLeftQuarterTurn5TilesUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &OpenRCT2::LatticeTriangle::kRightQuarterTurn5TilesUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &OpenRCT2::LatticeTriangle::kSBendLeftSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &OpenRCT2::LatticeTriangle::kSBendRightSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { &OpenRCT2::LatticeTriangle::kLeftVerticalLoopSprites[0][0], &OpenRCT2::LatticeTriangle::kLeftVerticalLoopOffsets[0][0], &OpenRCT2::LatticeTriangle::kLeftVerticalLoopBoundBoxes[0][0], false }, // LeftVerticalLoop
        { &OpenRCT2::LatticeTriangle::kRightVerticalLoopSprites[0][0], &OpenRCT2::LatticeTriangle::kRightVerticalLoopOffsets[0][0], &OpenRCT2::LatticeTriangle::kRightVerticalLoopBoundBoxes[0][0], false }, // RightVerticalLoop
        { &OpenRCT2::LatticeTriangle::kLeftQuarterTurn3TilesSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &OpenRCT2::LatticeTriangle::kLeftQuarterTurn3TilesUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kLeftQuarterTurn3TilesUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kLeftQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn3TilesUp25
        { &OpenRCT2::LatticeTriangle::kRightQuarterTurn3TilesUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kRightQuarterTurn3TilesUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false }, // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true }, // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true }, // RightTwistUpToDown
        { &OpenRCT2::LatticeTriangle::kHalfLoopUpSprites[0][0], &OpenRCT2::LatticeTriangle::kHalfLoopUpOffsets[0][0], &OpenRCT2::LatticeTriangle::kHalfLoopUpBoundBoxes[0][0], false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true }, // HalfLoopDown
        { &OpenRCT2::LatticeTriangle::kLeftCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftCorkscrewUpBoundBoxes[0][0], false }, // LeftCorkscrewUp
        { &OpenRCT2::LatticeTriangle::kRightCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightCorkscrewUpBoundBoxes[0][0], false }, // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightCorkscrewDown
        { &OpenRCT2::LatticeTriangle::kFlatToUp60Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToUp60BoundBoxes[0][0], false }, // FlatToUp60
        { &OpenRCT2::LatticeTriangle::kUp60ToFlatSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp60ToFlatBoundBoxes[0][0], false }, // Up60ToFlat
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
        { &OpenRCT2::LatticeTriangle::kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpSmall
        { &OpenRCT2::LatticeTriangle::kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &OpenRCT2::LatticeTriangle::kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpLarge
        { &OpenRCT2::LatticeTriangle::kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownLarge
        { &OpenRCT2::LatticeTriangle::kLeftQuarterTurn1TileUp60Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftQuarterTurn1TileUp60BoundBoxes[0][0], false }, // LeftQuarterTurn1TileUp60
        { &OpenRCT2::LatticeTriangle::kRightQuarterTurn1TileUp60Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightQuarterTurn1TileUp60BoundBoxes[0][0], false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0][0], nullptr, &kBrakesBoundBoxes[0], false }, // Brakes
        { &kBoosterSprites[0], nullptr, &kBoosterBoundBoxes[0], false }, // Booster
        { nullptr, nullptr, nullptr, false }, // Maze
        { nullptr, nullptr, nullptr, false }, // LeftQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, false }, // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, true }, // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, true }, // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, false }, // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, false }, // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, true }, // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, true }, // RightQuarterHelixLargeDown
        { &OpenRCT2::LatticeTriangle::kUp25LeftBankedSprites[0], nullptr, &OpenRCT2::LatticeTriangle::kUp25LeftBankedBoundBoxes[0], false }, // Up25LeftBanked
        { &OpenRCT2::LatticeTriangle::kUp25RightBankedSprites[0], nullptr, &OpenRCT2::LatticeTriangle::kUp25RightBankedBoundBoxes[0], false }, // Up25RightBanked
        { nullptr, nullptr, nullptr, false }, // Waterfall
        { nullptr, nullptr, nullptr, false }, // Rapids
        { &OpenRCT2::LatticeTriangle::kFlatSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kStationBoundBoxes[0], false }, // OnRidePhoto
        { nullptr, nullptr, nullptr, true }, // Down25LeftBanked
        { nullptr, nullptr, nullptr, true }, // Down25RightBanked
        { nullptr, nullptr, nullptr, false }, // Watersplash
        { &OpenRCT2::LatticeTriangle::kFlatToUp60LongBaseSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToUp60LongBaseBoundBoxes[0][0], false }, // FlatToUp60LongBase
        { &OpenRCT2::LatticeTriangle::kUp60ToFlatLongBaseSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp60ToFlatLongBaseBoundBoxes[0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // Whirlpool
        { nullptr, nullptr, nullptr, true }, // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // FlatToDown60LongBase
        { &OpenRCT2::LatticeTriangle::kCableLiftHillSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kCableLiftHillBoundBoxes[0][0][0], false }, // CableLiftHill
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallVertical
        { &OpenRCT2::LatticeTriangle::kUp90Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp90BoundBoxes[0][0], false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { &OpenRCT2::LatticeTriangle::kUp60ToUp90Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp60ToUp90BoundBoxes[0][0], false }, // Up60ToUp90
        { &OpenRCT2::LatticeTriangle::kDown90ToDown60Sprites[0], nullptr, &OpenRCT2::LatticeTriangle::kDown90ToDown60BoundBoxes[0], false }, // Down90ToDown60
        { &OpenRCT2::LatticeTriangle::kUp90ToUp60Sprites[0], nullptr, &OpenRCT2::LatticeTriangle::kUp90ToUp60BoundBoxes[0], false }, // Up90ToUp60
        { &OpenRCT2::LatticeTriangle::kDown60ToDown90Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kDown60ToDown90BoundBoxes[0][0], false }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &OpenRCT2::LatticeTriangle::kLeftEighthToDiagSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &OpenRCT2::LatticeTriangle::kRightEighthToDiagSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { &OpenRCT2::LatticeTriangle::kLeftEighthBankToDiagSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftEighthBankToDiagBoundBoxes[0][0], false }, // LeftEighthBankToDiag
        { &OpenRCT2::LatticeTriangle::kRightEighthBankToDiagSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightEighthBankToDiagBoundBoxes[0][0], false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &OpenRCT2::LatticeTriangle::kDiagFlatSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &OpenRCT2::LatticeTriangle::kDiagUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { &OpenRCT2::LatticeTriangle::kDiagUp60Sprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60BoundBoxes[0][0], false }, // DiagUp60
        { &OpenRCT2::LatticeTriangle::kDiagFlatToUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { &OpenRCT2::LatticeTriangle::kDiagUp25ToUp60Sprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToUp60Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToUp60BoundBoxes[0][0], false }, // DiagUp25ToUp60
        { &OpenRCT2::LatticeTriangle::kDiagUp60ToUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60ToUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60ToUp25BoundBoxes[0][0], false }, // DiagUp60ToUp25
        { &OpenRCT2::LatticeTriangle::kDiagUp25ToFlatSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToFlatOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown60
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToFlat
        { &OpenRCT2::LatticeTriangle::kDiagFlatToUp60Sprites[0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToUp60Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToUp60BoundBoxes[0][0], false }, // DiagFlatToUp60
        { &OpenRCT2::LatticeTriangle::kDiagUp60ToFlatSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60ToFlatOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60ToFlatBoundBoxes[0][0], false }, // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlat
        { &OpenRCT2::LatticeTriangle::kDiagFlatToLeftBankSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToLeftBankOffsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToLeftBankBoundBoxes[0][0][0], false }, // DiagFlatToLeftBank
        { &OpenRCT2::LatticeTriangle::kDiagFlatToRightBankSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToRightBankOffsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToRightBankBoundBoxes[0][0][0], false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToFlat
        { &OpenRCT2::LatticeTriangle::kDiagLeftBankToUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankToUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankToUp25BoundBoxes[0][0][0], false }, // DiagLeftBankToUp25
        { &OpenRCT2::LatticeTriangle::kDiagRightBankToUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankToUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankToUp25BoundBoxes[0][0][0], false }, // DiagRightBankToUp25
        { &OpenRCT2::LatticeTriangle::kDiagUp25ToLeftBankSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToLeftBankOffsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToLeftBankBoundBoxes[0][0][0], false }, // DiagUp25ToLeftBank
        { &OpenRCT2::LatticeTriangle::kDiagUp25ToRightBankSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToRightBankOffsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToRightBankBoundBoxes[0][0][0], false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBank
        { &OpenRCT2::LatticeTriangle::kDiagLeftBankSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankBoundBoxes[0][0], false }, // DiagLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagRightBank
        { nullptr, nullptr, nullptr, false }, // LogFlumeReverser
        { nullptr, nullptr, nullptr, false }, // SpinningTunnel
        { &OpenRCT2::LatticeTriangle::kLeftBarrelRollUpToDownSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBarrelRollUpToDownBoundBoxes[0][0][0], false }, // LeftBarrelRollUpToDown
        { &OpenRCT2::LatticeTriangle::kRightBarrelRollUpToDownSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightBarrelRollUpToDownBoundBoxes[0][0][0], false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true }, // RightBarrelRollDownToUp
        { &OpenRCT2::LatticeTriangle::kLeftBankToLeftQuarterTurn3TilesUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kLeftBankToLeftQuarterTurn3TilesUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { &OpenRCT2::LatticeTriangle::kRightBankToRightQuarterTurn3TilesUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kRightBankToRightQuarterTurn3TilesUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kRightBankToRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25ToRightBank
        { &kPoweredLiftSprites[0], nullptr, &kPoweredLiftBoundBoxes[0], false }, // PoweredLift
        { &OpenRCT2::LatticeTriangle::kLeftLargeHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftLargeHalfLoopUp
        { &OpenRCT2::LatticeTriangle::kRightLargeHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightLargeHalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerTwistUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, false }, // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, false }, // RightFlyerTwistDown
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, false }, // LeftFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // RightFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // HeartLineTransferUp
        { nullptr, nullptr, nullptr, true }, // HeartLineTransferDown
        { nullptr, nullptr, nullptr, false }, // LeftHeartLineRoll
        { nullptr, nullptr, nullptr, false }, // RightHeartLineRoll
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleA
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleB
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleC
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleD
        { nullptr, nullptr, nullptr, false }, // MinigolfHoleE
        { nullptr, nullptr, nullptr, false }, // MultiDimInvertedFlatToDown90QuarterLoop
        { &OpenRCT2::LatticeTriangle::kUp90ToInvertedFlatQuarterLoopSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &OpenRCT2::LatticeTriangle::kInvertedFlatToDown90QuarterLoopSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0], false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0], nullptr, &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
        { &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn3TileUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn3TileUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn3TileUp25BoundBoxes[0][0], false }, // LeftBankedQuarterTurn3TileUp25
        { &OpenRCT2::LatticeTriangle::kRightBankedQuarterTurn3TileUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kRightBankedQuarterTurn3TileUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kRightBankedQuarterTurn3TileUp25BoundBoxes[0][0], false }, // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3TileDown25
        { &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn5TileUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankedQuarterTurn5TileUp25BoundBoxes[0][0], false }, // LeftBankedQuarterTurn5TileUp25
        { &OpenRCT2::LatticeTriangle::kRightBankedQuarterTurn5TileUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightBankedQuarterTurn5TileUp25BoundBoxes[0][0], false }, // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn5TileDown25
        { &OpenRCT2::LatticeTriangle::kUp25ToLeftBankedUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25ToLeftBankedUp25BoundBoxes[0][0], false }, // Up25ToLeftBankedUp25
        { &OpenRCT2::LatticeTriangle::kUp25ToRightBankedUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kUp25ToRightBankedUp25BoundBoxes[0][0], false }, // Up25ToRightBankedUp25
        { &OpenRCT2::LatticeTriangle::kLeftBankedUp25ToUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankedUp25ToUp25BoundBoxes[0][0], false }, // LeftBankedUp25ToUp25
        { &OpenRCT2::LatticeTriangle::kRightBankedUp25ToUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightBankedUp25ToUp25BoundBoxes[0][0], false }, // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToDown25
        { &OpenRCT2::LatticeTriangle::kLeftBankedFlatToLeftBankedUp25Sprites[0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankedFlatToLeftBankedUp25BoundBoxes[0], false }, // LeftBankedFlatToLeftBankedUp25
        { &OpenRCT2::LatticeTriangle::kRightBankedFlatToRightBankedUp25Sprites[0], nullptr, &OpenRCT2::LatticeTriangle::kRightBankedFlatToRightBankedUp25BoundBoxes[0], false }, // RightBankedFlatToRightBankedUp25
        { &OpenRCT2::LatticeTriangle::kLeftBankedUp25ToLeftBankedFlatSprites[0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankedUp25ToLeftBankedFlatBoundBoxes[0], false }, // LeftBankedUp25ToLeftBankedFlat
        { &OpenRCT2::LatticeTriangle::kRightBankedUp25ToRightBankedFlatSprites[0], nullptr, &OpenRCT2::LatticeTriangle::kRightBankedUp25ToRightBankedFlatBoundBoxes[0], false }, // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToRightBankedFlat
        { &OpenRCT2::LatticeTriangle::kFlatToLeftBankedUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToLeftBankedUp25BoundBoxes[0][0], false }, // FlatToLeftBankedUp25
        { &OpenRCT2::LatticeTriangle::kFlatToRightBankedUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kFlatToRightBankedUp25BoundBoxes[0][0], false }, // FlatToRightBankedUp25
        { &OpenRCT2::LatticeTriangle::kLeftBankedUp25ToFlatSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftBankedUp25ToFlatBoundBoxes[0][0], false }, // LeftBankedUp25ToFlat
        { &OpenRCT2::LatticeTriangle::kRightBankedUp25ToFlatSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightBankedUp25ToFlatBoundBoxes[0][0], false }, // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToFlat
        { &OpenRCT2::LatticeTriangle::kLeftQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // LeftQuarterTurn1TileUp90
        { &OpenRCT2::LatticeTriangle::kRightQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // RightQuarterTurn1TileUp90
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
        { &OpenRCT2::LatticeTriangle::kLeftLargeCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftLargeCorkscrewUpBoundBoxes[0][0], false }, // LeftLargeCorkscrewUp
        { &OpenRCT2::LatticeTriangle::kRightLargeCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightLargeCorkscrewUpBoundBoxes[0][0], false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { &OpenRCT2::LatticeTriangle::kLeftMediumHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftMediumHalfLoopUpBoundBoxes[0][0], false }, // LeftMediumHalfLoopUp
        { &OpenRCT2::LatticeTriangle::kRightMediumHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightMediumHalfLoopUpBoundBoxes[0][0], false }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightMediumHalfLoopDown
        { &OpenRCT2::LatticeTriangle::kLeftZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftZeroGRollUpBoundBoxes[0][0][0], false }, // LeftZeroGRollUp
        { &OpenRCT2::LatticeTriangle::kRightZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightZeroGRollUpBoundBoxes[0][0][0], false }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true }, // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true }, // RightZeroGRollDown
        { &OpenRCT2::LatticeTriangle::kLeftLargeZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftLargeZeroGRollUpBoundBoxes[0][0][0], false }, // LeftLargeZeroGRollUp
        { &OpenRCT2::LatticeTriangle::kRightLargeZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightLargeZeroGRollUpBoundBoxes[0][0][0], false }, // RightLargeZeroGRollUp
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
        { &OpenRCT2::LatticeTriangle::kLeftEighthToDiagUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftEighthToDiagUp25BoundBoxes[0][0], false }, // LeftEighthToDiagUp25
        { &OpenRCT2::LatticeTriangle::kRightEighthToDiagUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightEighthToDiagUp25BoundBoxes[0][0], false }, // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToDiagDown25
        { &OpenRCT2::LatticeTriangle::kLeftEighthToOrthogonalUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftEighthToOrthogonalUp25BoundBoxes[0][0], false }, // LeftEighthToOrthogonalUp25
        { &OpenRCT2::LatticeTriangle::kRightEighthToOrthogonalUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightEighthToOrthogonalUp25BoundBoxes[0][0], false }, // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonalDown25
        { &OpenRCT2::LatticeTriangle::kDiagUp25ToLeftBankedUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToLeftBankedUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToLeftBankedUp25BoundBoxes[0][0][0], false }, // DiagUp25ToLeftBankedUp25
        { &OpenRCT2::LatticeTriangle::kDiagUp25ToRightBankedUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToRightBankedUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagUp25ToRightBankedUp25BoundBoxes[0][0][0], false }, // DiagUp25ToRightBankedUp25
        { &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToUp25BoundBoxes[0][0][0], false }, // DiagLeftBankedUp25ToUp25
        { &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToUp25BoundBoxes[0][0][0], false }, // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToDown25
        { &OpenRCT2::LatticeTriangle::kDiagLeftBankedFlatToLeftBankedUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedFlatToLeftBankedUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedFlatToLeftBankedUp25BoundBoxes[0][0], false }, // DiagLeftBankedFlatToLeftBankedUp25
        { &OpenRCT2::LatticeTriangle::kDiagRightBankedFlatToRightBankedUp25Sprites[0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedFlatToRightBankedUp25Offsets[0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedFlatToRightBankedUp25BoundBoxes[0][0], false }, // DiagRightBankedFlatToRightBankedUp25
        { &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToLeftBankedFlatSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToLeftBankedFlatOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToLeftBankedFlatBoundBoxes[0][0], false }, // DiagLeftBankedUp25ToLeftBankedFlat
        { &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToRightBankedFlatSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToRightBankedFlatOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToRightBankedFlatBoundBoxes[0][0], false }, // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToRightBankedFlat
        { &OpenRCT2::LatticeTriangle::kDiagFlatToLeftBankedUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToLeftBankedUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToLeftBankedUp25BoundBoxes[0][0][0], false }, // DiagFlatToLeftBankedUp25
        { &OpenRCT2::LatticeTriangle::kDiagFlatToRightBankedUp25Sprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToRightBankedUp25Offsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToRightBankedUp25BoundBoxes[0][0][0], false }, // DiagFlatToRightBankedUp25
        { &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToFlatSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToFlatOffsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagLeftBankedUp25ToFlatBoundBoxes[0][0][0], false }, // DiagLeftBankedUp25ToFlat
        { &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToFlatSprites[0][0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToFlatOffsets[0][0][0], &OpenRCT2::LatticeTriangle::kDiagRightBankedUp25ToFlatBoundBoxes[0][0][0], false }, // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToFlat
        { &OpenRCT2::LatticeTriangle::kDiagUp25LeftBankedSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25LeftBankedOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25LeftBankedBoundBoxes[0][0], false }, // DiagUp25LeftBanked
        { &OpenRCT2::LatticeTriangle::kDiagUp25RightBankedSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25RightBankedOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp25RightBankedBoundBoxes[0][0], false }, // DiagUp25RightBanked
        { nullptr, nullptr, nullptr, true }, // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr, true }, // DiagDown25RightBanked
        { &OpenRCT2::LatticeTriangle::kLeftEighthBankToDiagUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftEighthBankToDiagUp25BoundBoxes[0][0], false }, // LeftEighthBankToDiagUp25
        { &OpenRCT2::LatticeTriangle::kRightEighthBankToDiagUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightEighthBankToDiagUp25BoundBoxes[0][0], false }, // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToDiagDown25
        { &OpenRCT2::LatticeTriangle::kLeftEighthBankToOrthogonalUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kLeftEighthBankToOrthogonalUp25BoundBoxes[0][0], false }, // LeftEighthBankToOrthogonalUp25
        { &OpenRCT2::LatticeTriangle::kRightEighthBankToOrthogonalUp25Sprites[0][0], nullptr, &OpenRCT2::LatticeTriangle::kRightEighthBankToOrthogonalUp25BoundBoxes[0][0], false }, // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonalDown25
        { &kDiagBrakesSprites[0][0][0], &kDiagBrakesOffsets[0][0], &kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0], &kDiagBlockBrakesOffsets[0][0], &kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
        { nullptr, nullptr, nullptr, false }, // Down25Brakes
        { &kDiagBoosterSprites[0][0], &kDiagBoosterOffsets[0][0], &kDiagBoosterBoundBoxes[0][0], false }, // DiagBooster
        { &OpenRCT2::LatticeTriangle::kDiagFlatToUp60LongBaseSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToUp60LongBaseOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagFlatToUp60LongBaseBoundBoxes[0][0], false }, // DiagFlatToUp60LongBase
        { &OpenRCT2::LatticeTriangle::kDiagUp60ToFlatLongBaseSprites[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60ToFlatLongBaseOffsets[0][0], &OpenRCT2::LatticeTriangle::kDiagUp60ToFlatLongBaseBoundBoxes[0][0], false }, // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlatLongBase
        { &OpenRCT2::LatticeTriangle::kLeftEighthDiveLoopUpToOrthogonalSprites[0][0], &OpenRCT2::LatticeTriangle::kLeftEighthDiveLoopUpToOrthogonalOffsets[0][0], &OpenRCT2::LatticeTriangle::kLeftEighthDiveLoopUpToOrthogonalBoundBoxes[0][0], false }, // LeftEighthDiveLoopUpToOrthogonal
        { &OpenRCT2::LatticeTriangle::kRightEighthDiveLoopUpToOrthogonalSprites[0][0], &OpenRCT2::LatticeTriangle::kRightEighthDiveLoopUpToOrthogonalOffsets[0][0], &OpenRCT2::LatticeTriangle::kRightEighthDiveLoopUpToOrthogonalBoundBoxes[0][0], false }, // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, true }, // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, true }, // RightEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, false }, // DiagDown25Brakes
    } };
    // clang-format on
} // namespace OpenRCT2::LatticeTriangleAlt
