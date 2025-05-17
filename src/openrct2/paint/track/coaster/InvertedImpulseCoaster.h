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

namespace OpenRCT2::InvertedImpulseCoaster
{
    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatSprites = {{
        19662,
        19663,
        19662,
        19663,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            {{ 19662, SPR_STATION_INVERTED_BAR_B_SW_NE, }},
            {{ 19663, SPR_STATION_INVERTED_BAR_B_NW_SE, }},
            {{ 19662, SPR_STATION_INVERTED_BAR_B_SW_NE, }},
            {{ 19663, SPR_STATION_INVERTED_BAR_B_NW_SE, }},
        }},
        {{
            {{ 19662, kImageIndexUndefined, }},
            {{ 19663, kImageIndexUndefined, }},
            {{ 19662, kImageIndexUndefined, }},
            {{ 19663, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kStationOffsets = {{
        {{ { 0, 0, 29 }, { 0, 6, 29 } }},
        {{ { 0, 0, 29 }, { 6, 0, 29 } }},
        {{ { 0, 0, 29 }, { 0, 6, 29 } }},
        {{ { 0, 0, 29 }, { 6, 0, 29 } }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        {{ { { 0, 6, 29 }, { 32, 20, 3 } }, { { 0, 6, 29 }, { 32, 20, 3 } } }},
        {{ { { 6, 0, 29 }, { 20, 32, 3 } }, { { 6, 0, 29 }, { 20, 32, 3 } } }},
        {{ { { 0, 6, 29 }, { 32, 20, 3 } }, { { 0, 6, 29 }, { 32, 20, 3 } } }},
        {{ { { 6, 0, 29 }, { 20, 32, 3 } }, { { 6, 0, 29 }, { 20, 32, 3 } } }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25Sprites = {{
        19672,
        19673,
        19674,
        19675,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp60Sprites = {{
        19688,
        19689,
        19690,
        19691,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp60Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        { { 0, 6, 93 }, { 32, 20, 3 } },
        { { 4, 0, 11 }, { 2, 32, 81 } },
        { { 0, 4, 11 }, { 32, 2, 81 } },
        { { 6, 0, 93 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        19664,
        19665,
        19666,
        19667,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ 19676, kImageIndexUndefined, }},
        {{ 19680, 19677, }},
        {{ 19681, 19678, }},
        {{ 19679, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Offsets = {{
        {{ { 0, 0, 29 }, {} }},
        {{ { 0, 0, 29 }, { 0, 0, 29 } }},
        {{ { 0, 0, 29 }, { 0, 0, 29 } }},
        {{ { 0, 0, 29 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 61 }, { 32, 20, 3 } }, {} }},
        {{ { { 10, 0, 11 }, { 10, 32, 49 } }, { { 4, 0, 11 }, { 2, 32, 49 } } }},
        {{ { { 0, 10, 11 }, { 32, 10, 49 } }, { { 0, 4, 11 }, { 32, 2, 49 } } }},
        {{ { { 6, 0, 61 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Sprites = {{
        {{ 19682, kImageIndexUndefined, }},
        {{ 19686, 19683, }},
        {{ 19687, 19684, }},
        {{ 19685, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Offsets = {{
        {{ { 0, 0, 29 }, {} }},
        {{ { 0, 0, 29 }, { 0, 0, 29 } }},
        {{ { 0, 0, 29 }, { 0, 0, 29 } }},
        {{ { 0, 0, 29 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 61 }, { 32, 20, 3 } }, {} }},
        {{ { { 10, 0, 11 }, { 10, 32, 49 } }, { { 4, 0, 11 }, { 2, 32, 49 } } }},
        {{ { { 0, 10, 11 }, { 32, 10, 49 } }, { { 0, 4, 11 }, { 32, 2, 49 } } }},
        {{ { { 6, 0, 61 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        19668,
        19669,
        19670,
        19671,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToFlatOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp90Sprites = {{
        {{
            19700,
            kImageIndexUndefined,
        }},
        {{
            19701,
            kImageIndexUndefined,
        }},
        {{
            19702,
            kImageIndexUndefined,
        }},
        {{
            19703,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp90Offsets = {{
        {{
            { 0, 0, 29 }, 
            {},
        }},
        {{
            { 0, 0, 29 }, 
            {},
        }},
        {{
            { 0, 0, 29 }, 
            {},
        }},
        {{
            { 0, 0, 29 }, 
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp90BoundBoxes = {{
        {{
            { { 0, 6, 61 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 4, 0, 11 }, { 2, 32, 31 } },
            {},
        }},
        {{
            { { 0, 4, 11 }, { 32, 2, 31 } },
            {},
        }},
        {{
            { { 6, 0, 61 }, { 20, 32, 3 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp90Sprites = {{
        {{
            19692,
            kImageIndexUndefined,
        }},
        {{
            19693,
            kImageIndexUndefined,
        }},
        {{
            19694,
            kImageIndexUndefined,
        }},
        {{
            19695,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp90Offsets = {{
        {{
            { 0, 0, 29 }, 
            {},
        }},
        {{
            { 0, 0, 29 }, 
            {},
        }},
        {{
            { 0, 0, 29 }, 
            {},
        }},
        {{
            { 0, 0, 29 }, 
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp90BoundBoxes = {{
        {{
            { { 0, 6, 85 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 4, 0, 11 }, { 2, 32, 55 } },
            {},
        }},
        {{
            { { 0, 4, 11 }, { 32, 2, 55 } },
            {},
        }},
        {{
            { { 6, 0, 85 }, { 20, 32, 3 } },
            {},
        }},
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kDown90ToDown60Sprites = {{
        19694,
        19695,
        19692,
        19693,
    }};

    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kDown90ToDown60Offsets = {{
        { 0, 0, 29 },
        { 0, 0, 29 },
        { 0, 0, 29 },
        { 0, 0, 29 },
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kDown90ToDown60BoundBoxes = {{
        { { 0, 4, 11 }, { 32, 2, 55 } },
        { { 6, 0, 85 }, { 20, 32, 3 } },
        { { 0, 6, 85 }, { 32, 20, 3 } },
        { { 4, 0, 11 }, { 2, 32, 55 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp90ToUp60Sprites = {{
        19696,
        19697,
        19698,
        19699,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp90ToUp60Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp90ToUp60BoundBoxes = {{
        { { 0, 6, 85 }, { 32, 20, 3 } },
        { { 4, 0, 11 }, { 2, 32, 55 } },
        { { 0, 4, 11 }, { 32, 2, 55 } },
        { { 6, 0, 85 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Sprites = {{
        {{
            19698,
            kImageIndexUndefined,
        }},
        {{
            19699,
            kImageIndexUndefined,
        }},
        {{
            19696,
            kImageIndexUndefined,
        }},
        {{
            19697,
            kImageIndexUndefined,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Offsets = {{
        {{
            { 0, 0, 29 },
            {},
        }},
        {{
            { 0, 0, 29 },
            {},
        }},
        {{
            { 0, 0, 29 },
            {},
        }},
        {{
            { 0, 0, 29 },
            {},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90BoundBoxes = {{
        {{
            { { 0, 4, 11 }, { 32, 2, 55 } },
            {},
        }},
        {{
            { { 6, 0, 85 }, { 20, 32, 3 } },
            {},
        }},
        {{
            { { 0, 6, 85 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 4, 0, 11 }, { 2, 32, 55 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ 19708, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 19709, 19717, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 19710, 19718, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 19711, 19719, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Offsets = {{
        {{
            {{ { 0, 0, 29 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { 0, 0, 29 }, { 0, 0, 29 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { 0, 0, 29 }, { 0, 0, 29 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { 0, 0, 29 }, { 0, 0, 29 } }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90BoundBoxes = {{
        {{
            {{ { { 0, 6, 125 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 0, 125 }, { 20, 32, 3 } }, { { 0, 4, 11 }, { 32, 2, 31 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 0, 6, 125 }, { 32, 20, 3 } }, { { 0, 4, 11 }, { 32, 2, 31 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 0, 125 }, { 20, 32, 3 } }, { { 4, 0, 11 }, { 2, 32, 31 } } }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90Sprites = {{
        {{
            {{ 19704, 19712, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 19705, 19713, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 19706, 19714, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 19707, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90Offsets = {{
        {{
            {{ { 0, 0, 29 }, { 0, 0, 29 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { 0, 0, 29 }, { 0, 0, 29 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { 0, 0, 29 }, { 0, 0, 29 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { 0, 0, 29 }, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0], &kUp25Offsets[0], &kUp25BoundBoxes[0], false }, // Up25
        { &kUp60Sprites[0], &kUp60Offsets[0], &kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0], &kUp25ToUp60Offsets[0][0], &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0], &kUp60ToUp25Offsets[0][0], &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0], &kUp25ToFlatOffsets[0], &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn5Tiles
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
        { nullptr, nullptr, nullptr, false }, // SBendLeft
        { nullptr, nullptr, nullptr, false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn3Tiles
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
        { nullptr, nullptr, nullptr, false }, // Brakes
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
        { &kUp90Sprites[0][0], &kUp90Offsets[0][0], &kUp90BoundBoxes[0][0], false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { &kUp60ToUp90Sprites[0][0], &kUp60ToUp90Offsets[0][0], &kUp60ToUp90BoundBoxes[0][0], false }, // Up60ToUp90
        { &kDown90ToDown60Sprites[0], &kDown90ToDown60Offsets[0], &kDown90ToDown60BoundBoxes[0], false }, // Down90ToDown60
        { &kUp90ToUp60Sprites[0], &kUp90ToUp60Offsets[0], &kUp90ToUp60BoundBoxes[0], false }, // Up90ToUp60
        { &kDown60ToDown90Sprites[0][0], &kDown60ToDown90Offsets[0][0], &kDown60ToDown90BoundBoxes[0][0], false }, // Down60ToDown90
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
        { nullptr, nullptr, nullptr, false }, // LeftReverser
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
        { &kLeftQuarterTurn1TileUp90Sprites[0][0][0], &kLeftQuarterTurn1TileUp90Offsets[0][0][0], &kLeftQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // LeftQuarterTurn1TileUp90
        { &kRightQuarterTurn1TileUp90Sprites[0][0][0], &kRightQuarterTurn1TileUp90Offsets[0][0][0], &kRightQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // RightQuarterTurn1TileUp90
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
} // namespace OpenRCT2::InvertedImpulseCoaster
