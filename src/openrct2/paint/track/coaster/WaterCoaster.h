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
#include "../../../paint/track/coaster/JuniorRollerCoaster.h"
#include "../../../ride/RideData.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::WaterCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            27807,
            27808,
            27807,
            27808,
        }},
        {{
            27983,
            27984,
            27983,
            27984,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            27809,
            27821,
            27813,
            27823,
        }},
        {{
            27993,
            27994,
            27995,
            27996,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            27811,
            27825,
            27815,
            27827,
        }},
        {{
            27985,
            27986,
            27987,
            27988,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            27819,
            27831,
            27817,
            27829,
        }},
        {{
            27989,
            27990,
            27991,
            27992,
        }},
    }};

    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToFlatOffsets = {{
        { 0, 6, 0 },
        { 6, 0, 0 },
        { 0, 6, 0 },
        { 6, 0, 0 },
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                28197,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28198,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28199,
                kImageIndexUndefined,
            }},
            {{
                28200,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28269,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28270,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28271,
                kImageIndexUndefined,
            }},
            {{
                28272,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatOffsets = {{
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
    kDiagFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                28209,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28210,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28211,
                kImageIndexUndefined,
            }},
            {{
                28212,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28281,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28282,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28283,
                kImageIndexUndefined,
            }},
            {{
                28284,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25Offsets = {{
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
    kDiagUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                28201,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28202,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28203,
                kImageIndexUndefined,
            }},
            {{
                28204,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28273,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28274,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28275,
                kImageIndexUndefined,
            }},
            {{
                28276,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25Offsets = {{
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
    kDiagFlatToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25ToFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                28205,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28206,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28207,
                kImageIndexUndefined,
            }},
            {{
                28208,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28277,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28278,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28279,
                kImageIndexUndefined,
            }},
            {{
                28280,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatOffsets = {{
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
    kDiagUp25ToFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &OpenRCT2::JuniorRollerCoaster::kStationSprites[0], &OpenRCT2::JuniorRollerCoaster::kFlatOffsets[0], &OpenRCT2::JuniorRollerCoaster::kStationBoundBoxes[0], false }, // EndStation
        { &OpenRCT2::JuniorRollerCoaster::kStationSprites[0], &OpenRCT2::JuniorRollerCoaster::kFlatOffsets[0], &OpenRCT2::JuniorRollerCoaster::kStationBoundBoxes[0], false }, // BeginStation
        { &OpenRCT2::JuniorRollerCoaster::kStationSprites[0], &OpenRCT2::JuniorRollerCoaster::kFlatOffsets[0], &OpenRCT2::JuniorRollerCoaster::kStationBoundBoxes[0], false }, // MiddleStation
        { &kUp25Sprites[0][0], &kUp25Offsets[0], &kUp25BoundBoxes[0], false }, // Up25
        { &OpenRCT2::JuniorRollerCoaster::kUp60Sprites[0][0], &OpenRCT2::JuniorRollerCoaster::kUp60Offsets[0], &OpenRCT2::JuniorRollerCoaster::kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &OpenRCT2::JuniorRollerCoaster::kUp25ToUp60Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kUp25ToUp60Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &OpenRCT2::JuniorRollerCoaster::kUp60ToUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kUp60ToUp25Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], &kUp25ToFlatOffsets[0], &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5Tiles
        { &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn5TilesSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn5TilesOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn5TilesBoundBoxes[0][0], false }, // RightQuarterTurn5Tiles
        { &OpenRCT2::JuniorRollerCoaster::kFlatToLeftBankSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kFlatToLeftBankBoundBoxes[0][0], false }, // FlatToLeftBank
        { &OpenRCT2::JuniorRollerCoaster::kFlatToRightBankSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { nullptr, nullptr, nullptr, true }, // BankedLeftQuarterTurn5Tiles
        { &OpenRCT2::JuniorRollerCoaster::kBankedRightQuarterTurn5TilesSprites[0][0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kBankedRightQuarterTurn5TilesBoundBoxes[0][0][0], false }, // BankedRightQuarterTurn5Tiles
        { &OpenRCT2::JuniorRollerCoaster::kLeftBankToUp25Sprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kLeftBankToUp25BoundBoxes[0][0], false }, // LeftBankToUp25
        { &OpenRCT2::JuniorRollerCoaster::kRightBankToUp25Sprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightBankToUp25BoundBoxes[0][0], false }, // RightBankToUp25
        { &OpenRCT2::JuniorRollerCoaster::kUp25ToLeftBankSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kUp25ToLeftBankBoundBoxes[0][0], false }, // Up25ToLeftBank
        { &OpenRCT2::JuniorRollerCoaster::kUp25ToRightBankSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kUp25ToRightBankBoundBoxes[0][0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBank
        { &OpenRCT2::JuniorRollerCoaster::kLeftBankSprites[0], nullptr, &OpenRCT2::JuniorRollerCoaster::kLeftBankBoundBoxes[0], false }, // LeftBank
        { nullptr, nullptr, nullptr, true }, // RightBank
        { &OpenRCT2::JuniorRollerCoaster::kLeftQuarterTurn5TilesUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kLeftQuarterTurn5TilesUp25Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn5TilesUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn5TilesUp25Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &OpenRCT2::JuniorRollerCoaster::kSBendLeftSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kSBendLeftOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &OpenRCT2::JuniorRollerCoaster::kSBendRightSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kSBendRightOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3Tiles
        { &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesBoundBoxes[0][0], false }, // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3Tiles
        { &OpenRCT2::JuniorRollerCoaster::kRightBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightBankedQuarterTurn3TilesBoundBoxes[0][0][0], false }, // RightBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesUp25
        { &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesUp25Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesDown25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesDown25Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kRightQuarterTurn3TilesDown25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesDown25
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
        { &OpenRCT2::JuniorRollerCoaster::kFlatToUp60Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kFlatToUp60Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kFlatToUp60BoundBoxes[0][0], false }, // FlatToUp60
        { &OpenRCT2::JuniorRollerCoaster::kUp60ToFlatSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kUp60ToFlatOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kUp60ToFlatBoundBoxes[0][0], false }, // Up60ToFlat
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
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixUpSmall
        { &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixDownSmallSprites[0][0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixDownSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixUpLarge
        { &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixDownLargeSprites[0][0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightHalfBankedHelixDownLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &OpenRCT2::JuniorRollerCoaster::kBrakesSprites[0], &OpenRCT2::JuniorRollerCoaster::kBrakesOffsets[0], &OpenRCT2::JuniorRollerCoaster::kBrakesBoundBoxes[0], false }, // Brakes
        { &OpenRCT2::JuniorRollerCoaster::kBoosterSprites[0], nullptr, &OpenRCT2::JuniorRollerCoaster::kBoosterBoundBoxes[0], false }, // Booster
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
        { &kFlatSprites[0][0], &kFlatOffsets[0], &OpenRCT2::JuniorRollerCoaster::kStationBoundBoxes[0], false }, // OnRidePhoto
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
        { nullptr, nullptr, nullptr, false }, // Down90ToDown60
        { nullptr, nullptr, nullptr, false }, // Up90ToUp60
        { nullptr, nullptr, nullptr, false }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &OpenRCT2::JuniorRollerCoaster::kLeftEighthToDiagSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &OpenRCT2::JuniorRollerCoaster::kRightEighthToDiagSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { &OpenRCT2::JuniorRollerCoaster::kLeftEighthBankToDiagSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kLeftEighthBankToDiagBoundBoxes[0][0], false }, // LeftEighthBankToDiag
        { &OpenRCT2::JuniorRollerCoaster::kRightEighthBankToDiagSprites[0][0], nullptr, &OpenRCT2::JuniorRollerCoaster::kRightEighthBankToDiagBoundBoxes[0][0], false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0], &kDiagFlatOffsets[0][0], &kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0], &kDiagUp25Offsets[0][0], &kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { &OpenRCT2::JuniorRollerCoaster::kDiagUp60Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp60Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp60BoundBoxes[0][0], false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToUp60Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToUp60Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToUp60BoundBoxes[0][0], false }, // DiagUp25ToUp60
        { &OpenRCT2::JuniorRollerCoaster::kDiagUp60ToUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp60ToUp25Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp60ToUp25BoundBoxes[0][0], false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0], &kDiagUp25ToFlatOffsets[0][0], &kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown60
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToFlat
        { &OpenRCT2::JuniorRollerCoaster::kDiagFlatToUp60Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagFlatToUp60Offsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagFlatToUp60BoundBoxes[0][0], false }, // DiagFlatToUp60
        { &OpenRCT2::JuniorRollerCoaster::kDiagUp60ToFlatSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp60ToFlatOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp60ToFlatBoundBoxes[0][0], false }, // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlat
        { &OpenRCT2::JuniorRollerCoaster::kDiagFlatToLeftBankSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagFlatToLeftBankOffsets[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagFlatToLeftBankBoundBoxes[0][0][0], false }, // DiagFlatToLeftBank
        { &OpenRCT2::JuniorRollerCoaster::kDiagFlatToRightBankSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagFlatToRightBankOffsets[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagFlatToRightBankBoundBoxes[0][0][0], false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToFlat
        { &OpenRCT2::JuniorRollerCoaster::kDiagLeftBankToUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagLeftBankToUp25Offsets[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagLeftBankToUp25BoundBoxes[0][0][0], false }, // DiagLeftBankToUp25
        { &OpenRCT2::JuniorRollerCoaster::kDiagRightBankToUp25Sprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagRightBankToUp25Offsets[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagRightBankToUp25BoundBoxes[0][0][0], false }, // DiagRightBankToUp25
        { &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToLeftBankSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToLeftBankOffsets[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToLeftBankBoundBoxes[0][0][0], false }, // DiagUp25ToLeftBank
        { &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToRightBankSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToRightBankOffsets[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagUp25ToRightBankBoundBoxes[0][0][0], false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBank
        { &OpenRCT2::JuniorRollerCoaster::kDiagLeftBankSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagLeftBankOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagLeftBankBoundBoxes[0][0], false }, // DiagLeftBank
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
        { nullptr, nullptr, nullptr, true }, // HeartLineTransferDown
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
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &OpenRCT2::JuniorRollerCoaster::kBlockBrakesSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kBlockBrakesOffsets[0], &OpenRCT2::JuniorRollerCoaster::kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
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
        { &OpenRCT2::JuniorRollerCoaster::kDiagBrakesSprites[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagBrakesOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &OpenRCT2::JuniorRollerCoaster::kDiagBlockBrakesSprites[0][0][0], &OpenRCT2::JuniorRollerCoaster::kDiagBlockBrakesOffsets[0][0], &OpenRCT2::JuniorRollerCoaster::kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
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
} // namespace OpenRCT2::WaterCoaster
