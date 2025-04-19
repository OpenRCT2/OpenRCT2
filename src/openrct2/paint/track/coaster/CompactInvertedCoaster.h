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

namespace OpenRCT2::CompactInvertedCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            26555,
            26556,
            26555,
            26556,
        }},
        {{
            26557,
            26558,
            26557,
            26558,
        }},
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
            {{ 26557, SPR_STATION_INVERTED_BAR_A_SW_NE, }},
            {{ 26558, SPR_STATION_INVERTED_BAR_A_NW_SE, }},
            {{ 26557, SPR_STATION_INVERTED_BAR_A_SW_NE, }},
            {{ 26558, SPR_STATION_INVERTED_BAR_A_NW_SE, }},
        }},
        {{
            {{ 26557, kImageIndexUndefined, }},
            {{ 26558, kImageIndexUndefined, }},
            {{ 26557, kImageIndexUndefined, }},
            {{ 26558, kImageIndexUndefined, }},
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            26569,
            26570,
            26571,
            26572,
        }},
        {{
            26621,
            26622,
            26623,
            26624,
        }},
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp60Sprites = {{
        {{
            26585,
            26586,
            26587,
            26588,
        }},
        {{
            26637,
            26638,
            26639,
            26640,
        }},
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            26561,
            26562,
            26563,
            26564,
        }},
        {{
            26613,
            26614,
            26615,
            26616,
        }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToUp60Sprites = {{
        {{
            {{ 26573, kImageIndexUndefined, }},
            {{ 26577, 26574, }},
            {{ 26578, 26575, }},
            {{ 26576, kImageIndexUndefined, }},
        }},
        {{
            {{ 26625, kImageIndexUndefined, }},
            {{ 26629, 26626, }},
            {{ 26630, 26627, }},
            {{ 26628, kImageIndexUndefined, }},
        }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToUp25Sprites = {{
        {{
            {{ 26579, kImageIndexUndefined, }},
            {{ 26583, 26580, }},
            {{ 26584, 26581, }},
            {{ 26582, kImageIndexUndefined, }},
        }},
        {{
            {{ 26631, kImageIndexUndefined, }},
            {{ 26635, 26632, }},
            {{ 26636, 26633, }},
            {{ 26634, kImageIndexUndefined, }},
        }},
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            26565,
            26566,
            26567,
            26568,
        }},
        {{
            26617,
            26618,
            26619,
            26620,
        }},
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
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            26694,
            kImageIndexUndefined,
            26693,
            26692,
            kImageIndexUndefined,
            26691,
            26690,
        }},
        {{
            26699,
            kImageIndexUndefined,
            26698,
            26697,
            kImageIndexUndefined,
            26696,
            26695,
        }},
        {{
            26704,
            kImageIndexUndefined,
            26703,
            26702,
            kImageIndexUndefined,
            26701,
            26700,
        }},
        {{
            26689,
            kImageIndexUndefined,
            26688,
            26687,
            kImageIndexUndefined,
            26686,
            26685,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 32, 3 } },
            { { 16, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 16, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 0, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        26657,
        26658,
        26659,
        26660,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        26661,
        26662,
        26663,
        26664,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToRightBankOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            26738,
            kImageIndexUndefined,
            26737,
            26736,
            kImageIndexUndefined,
            26735,
            26734,
        }},
        {{
            26743,
            kImageIndexUndefined,
            26742,
            26741,
            kImageIndexUndefined,
            26740,
            26739,
        }},
        {{
            26748,
            kImageIndexUndefined,
            26747,
            26746,
            kImageIndexUndefined,
            26745,
            26744,
        }},
        {{
            26733,
            kImageIndexUndefined,
            26732,
            26731,
            kImageIndexUndefined,
            26730,
            26729,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 32, 3 } },
            { { 16, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 16, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 0, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        26665,
        26666,
        26667,
        26668,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        26669,
        26670,
        26671,
        26672,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankToUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        26673,
        26674,
        26675,
        26676,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        26677,
        26678,
        26679,
        26680,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        26681,
        26682,
        26683,
        26684,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            26949,
            kImageIndexUndefined,
            26950,
            26951,
            kImageIndexUndefined,
            26952,
            26953,
        }},
        {{
            26954,
            kImageIndexUndefined,
            26955,
            26956,
            kImageIndexUndefined,
            26957,
            26958,
        }},
        {{
            26959,
            kImageIndexUndefined,
            26960,
            26961,
            kImageIndexUndefined,
            26962,
            26963,
        }},
        {{
            26964,
            kImageIndexUndefined,
            26965,
            26966,
            kImageIndexUndefined,
            26967,
            26968,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Offsets = {{
        {{
            { 0, 6, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 16, 29 }, 
            {},
            { 16, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 16, 16, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            {},
            { 0, 16, 29 }, 
            { 16, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            { 16, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 16, 29 }, 
            { 0, 6, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25BoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 32, 3 } },
            { { 16, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 16, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 0, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            26929,
            kImageIndexUndefined,
            26930,
            26931,
            kImageIndexUndefined,
            26932,
            26933,
        }},
        {{
            26934,
            kImageIndexUndefined,
            26935,
            26936,
            kImageIndexUndefined,
            26937,
            26938,
        }},
        {{
            26939,
            kImageIndexUndefined,
            26940,
            26941,
            kImageIndexUndefined,
            26942,
            26943,
        }},
        {{
            26944,
            kImageIndexUndefined,
            26945,
            26946,
            kImageIndexUndefined,
            26947,
            26948,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Offsets = {{
        {{
            { 0, 6, 29 }, 
            {},
            { 0, 16, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 16, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            { 16, 0, 29 }, 
            { 0, 16, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 16, 16, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 16, 0, 29 }, 
            {},
            { 0, 16, 29 }, 
            { 0, 6, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            26641,
            26642,
            26643,
            26644,
        }},
        {{
            26645,
            26646,
            26647,
            26648,
        }},
        {{
            26644,
            26643,
            26642,
            26641,
        }},
        {{
            26648,
            26647,
            26646,
            26645,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 0, 29 }, { 32, 26, 3 } },
            { { 0, 6, 29 }, { 32, 26, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 0, 0, 29 }, { 26, 32, 3 } },
            { { 6, 0, 29 }, { 26, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 29 }, { 32, 26, 3 } },
            { { 0, 0, 29 }, { 32, 26, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 26, 32, 3 } },
            { { 0, 0, 29 }, { 26, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            26649,
            26650,
            26651,
            26652,
        }},
        {{
            26653,
            26654,
            26655,
            26656,
        }},
        {{
            26652,
            26651,
            26650,
            26649,
        }},
        {{
            26656,
            26655,
            26654,
            26653,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            27017,
            27018,
            27019,
            27020,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27021,
            27022,
            27023,
            27024,
        }},
        {{
            27025,
            27026,
            27027,
            27028,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27029,
            27030,
            27031,
            27032,
        }},
        {{
            27024,
            27023,
            27022,
            27021,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27020,
            27019,
            27018,
            27017,
        }},
        {{
            27032,
            27031,
            27030,
            27029,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27028,
            27027,
            27026,
            27025,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 0, 29 }, 
            { 16, 0, 2 }, 
            { 0, 0, 34 }, 
            {},
            {},
            { 0, 16, 34 }, 
            { 10, 16, 2 }, 
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 14, 0, 29 }, 
            { 0, 12, 2 }, 
            { 0, 0, 34 }, 
            {},
            {},
            { 16, 0, 34 }, 
            { 16, 16, 2 }, 
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 10, 16, 2 }, 
            { 0, 16, 34 }, 
            {},
            {},
            { 0, 0, 34 }, 
            { 16, 0, 2 }, 
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 16, 16, 2 }, 
            { 16, 0, 34 }, 
            {},
            {},
            { 0, 0, 34 }, 
            { 0, 12, 2 }, 
            { 14, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopBoundBoxes = {{
        {{
            { { 0, 29, 29 }, { 32, 2, 3 } },
            { { 0, 29, 29 }, { 32, 2, 3 } },
            { { 16, 29, 2 }, { 5, 2, 119 } },
            { { 0, 0, -6 }, { 32, 2, 3 } },
            {},
            {},
            { { 0, 0, -6 }, { 32, 2, 3 } },
            { { 10, 0, 2 }, { 4, 2, 119 } },
            { { 0, 0, 29 }, { 32, 2, 3 } },
            { { 0, 29, 29 }, { 32, 2, 3 } },
        }},
        {{
            { { 29, 0, 29 }, { 2, 32, 3 } },
            { { 29, 0, 29 }, { 2, 32, 63 } },
            { { -6, 0, 2 }, { 2, 32, 119 } },
            { { 0, 0, -6 }, { 2, 2, 3 } },
            {},
            {},
            { { 0, 0, -6 }, { 2, 2, 3 } },
            { { 29, 29, 2 }, { 2, 2, 119 } },
            { { 20, 29, 29 }, { 2, 2, 3 } },
            { { 6, 0, 45 }, { 10, 32, 7 } },
        }},
        {{
            { { 0, 29, 29 }, { 32, 2, 3 } },
            { { 0, 0, 29 }, { 32, 2, 3 } },
            { { 10, 0, 2 }, { 4, 2, 119 } },
            { { 0, 0, -6 }, { 32, 2, 3 } },
            {},
            {},
            { { 0, 0, -6 }, { 32, 2, 3 } },
            { { 16, 29, 2 }, { 5, 2, 119 } },
            { { 0, 29, 29 }, { 32, 2, 3 } },
            { { 0, 29, 29 }, { 32, 2, 3 } },
        }},
        {{
            { { 6, 0, 45 }, { 10, 32, 7 } },
            { { 20, 29, 29 }, { 2, 2, 3 } },
            { { 29, 29, 2 }, { 2, 2, 119 } },
            { { 0, 0, -6 }, { 2, 2, 3 } },
            {},
            {},
            { { 0, 0, -6 }, { 2, 2, 3 } },
            { { -6, 0, 2 }, { 2, 32, 119 } },
            { { 29, 0, 29 }, { 2, 32, 63 } },
            { { 29, 0, 29 }, { 2, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopSprites = {{
        {{
            27048,
            27047,
            27046,
            27045,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27044,
            27043,
            27042,
            27041,
        }},
        {{
            27040,
            27039,
            27038,
            27037,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27036,
            27035,
            27034,
            27033,
        }},
        {{
            27041,
            27042,
            27043,
            27044,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27045,
            27046,
            27047,
            27048,
        }},
        {{
            27033,
            27034,
            27035,
            27036,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27037,
            27038,
            27039,
            27040,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 16, 16, 2 }, 
            { 0, 16, 34 }, 
            {},
            {},
            { 0, 0, 34 }, 
            { 12, 0, 2 }, 
            { 0, 14, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 16, 10, 2 }, 
            { 16, 0, 34 }, 
            {},
            {},
            { 0, 0, 34 }, 
            { 0, 16, 2 }, 
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 14, 29 }, 
            { 12, 0, 2 }, 
            { 0, 0, 34 }, 
            {},
            {},
            { 0, 16, 34 }, 
            { 16, 16, 2 }, 
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 16, 2 }, 
            { 0, 0, 34 }, 
            {},
            {},
            { 16, 0, 34 }, 
            { 16, 10, 2 }, 
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftVerticalLoopBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            26710,
            kImageIndexUndefined,
            26709,
            26708,
        }},
        {{
            26713,
            kImageIndexUndefined,
            26712,
            26711,
        }},
        {{
            26716,
            kImageIndexUndefined,
            26715,
            26714,
        }},
        {{
            26707,
            kImageIndexUndefined,
            26706,
            26705,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesOffsets = {{
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 16, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 29 }, { 16, 16, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 29 }, { 16, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            26722,
            kImageIndexUndefined,
            26721,
            26720,
        }},
        {{
            26725,
            kImageIndexUndefined,
            26724,
            26723,
        }},
        {{
            26728,
            kImageIndexUndefined,
            26727,
            26726,
        }},
        {{
            26719,
            kImageIndexUndefined,
            26718,
            26717,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesOffsets = {{
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 29 }, { 16, 16, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 29 }, { 16, 16, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 29 }, { 16, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            26924,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26923,
        }},
        {{
            26926,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26925,
        }},
        {{
            26928,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26927,
        }},
        {{
            26922,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26921,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Offsets = {{
        {{
            { 0, 6, 29 }, 
            {},
            {},
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            {},
            { 0, 6, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            {},
            {},
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            {},
            { 0, 6, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25BoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            {},
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            {},
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            {},
            {},
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            {},
            {},
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            26913,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26914,
        }},
        {{
            26915,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26916,
        }},
        {{
            26917,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26918,
        }},
        {{
            26919,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26920,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Offsets = {{
        {{
            { 0, 6, 29 }, 
            {},
            {},
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            {},
            { 0, 6, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            {},
            {},
            { 6, 0, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            {},
            {},
            { 0, 6, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn3TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpSprites = {{
        {{
            26993,
            26994,
            26995,
        }},
        {{
            26999,
            27000,
            27001,
        }},
        {{
            26998,
            26997,
            26996,
        }},
        {{
            27004,
            27003,
            27002,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpSprites = {{
        {{
            27005,
            27006,
            27007,
        }},
        {{
            27011,
            27012,
            27013,
        }},
        {{
            27010,
            27009,
            27008,
        }},
        {{
            27016,
            27015,
            27014,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftTwistDownToUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kHalfLoopUpSprites = {{
        {{
            27073,
            27074,
            27075,
            27076,
        }},
        {{
            27081,
            27082,
            27083,
            27084,
        }},
        {{
            27080,
            27079,
            27078,
            27077,
        }},
        {{
            27088,
            27087,
            27086,
            27085,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kHalfLoopUpOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 0, 29 }, 
            { 16, 0, 2 }, 
            { 0, 0, 34 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 14, 0, 29 }, 
            { 0, 12, 2 }, 
            { 0, 0, 34 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 10, 16, 2 }, 
            { 0, 16, 34 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 16, 16, 2 }, 
            { 16, 0, 34 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kHalfLoopUpBoundBoxes = {{
        {{
            { { 0, 6, 45 }, { 32, 20, 3 } },
            { { 0, 29, 29 }, { 32, 2, 3 } },
            { { 16, 29, 2 }, { 5, 2, 119 } },
            { { 0, 6, 34 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 45 }, { 20, 32, 3 } },
            { { 29, 0, 29 }, { 2, 32, 63 } },
            { { -6, 0, 2 }, { 2, 32, 119 } },
            { { 0, 0, -6 }, { 2, 2, 3 } },
        }},
        {{
            { { 0, 6, 45 }, { 32, 20, 3 } },
            { { 0, 0, 29 }, { 32, 2, 3 } },
            { { 10, 0, 2 }, { 4, 2, 119 } },
            { { 0, 0, -6 }, { 32, 2, 3 } },
        }},
        {{
            { { 6, 0, 45 }, { 20, 32, 3 } },
            { { 20, 29, 29 }, { 2, 2, 3 } },
            { { 29, 29, 2 }, { 2, 2, 119 } },
            { { 6, 0, 34 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftCorkscrewUpSprites = {{
        {{
            27049,
            27050,
            27051,
        }},
        {{
            27052,
            27053,
            27054,
        }},
        {{
            27055,
            27056,
            27057,
        }},
        {{
            27058,
            27059,
            27060,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftCorkscrewUpOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftCorkscrewUpBoundBoxes = {{
        {{
            { { 0, 6, 33 }, { 32, 20, 3 } },
            { { 6, 6, 24 }, { 20, 20, 3 } },
            { { 6, 0, 39 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 33 }, { 20, 32, 3 } },
            { { 6, 6, 24 }, { 20, 20, 3 } },
            { { 0, 6, 39 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 33 }, { 32, 20, 3 } },
            { { 6, 6, 24 }, { 20, 20, 3 } },
            { { 6, 0, 39 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 33 }, { 20, 32, 3 } },
            { { 6, 6, 24 }, { 20, 20, 3 } },
            { { 0, 6, 39 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpSprites = {{
        {{
            27061,
            27062,
            27063,
        }},
        {{
            27064,
            27065,
            27066,
        }},
        {{
            27067,
            27068,
            27069,
        }},
        {{
            27070,
            27071,
            27072,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 14 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ 26910, 26906, }},
        {{ 26911, 26907, }},
        {{ 26912, 26908, }},
        {{ 26909, 26905, }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Offsets = {{
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60BoundBoxes = {{
        {{ { { 2, 2, 5 }, { 28, 28, 3 } }, { { 2, 2, 104 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 5 }, { 28, 28, 3 } }, { { 2, 2, 104 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 5 }, { 28, 28, 3 } }, { { 2, 2, 104 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 5 }, { 28, 28, 3 } }, { { 2, 2, 104 }, { 28, 28, 1 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60Sprites = {{
        {{ 26901, 26897, }},
        {{ 26902, 26898, }},
        {{ 26903, 26899, }},
        {{ 26904, 26900, }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60Offsets = {{
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
        {{ { 0, 0, 5 }, { 0, 0, 5 } }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        26559,
        26560,
        26559,
        26560,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBrakesOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterBankedHelixLargeUpSprites = {{
        {{
            27118,
            kImageIndexUndefined,
            27117,
            27116,
            kImageIndexUndefined,
            27115,
            27114,
        }},
        {{
            27123,
            kImageIndexUndefined,
            27122,
            27121,
            kImageIndexUndefined,
            27120,
            27119,
        }},
        {{
            27128,
            kImageIndexUndefined,
            27127,
            27126,
            kImageIndexUndefined,
            27125,
            27124,
        }},
        {{
            27113,
            kImageIndexUndefined,
            27112,
            27111,
            kImageIndexUndefined,
            27110,
            27109,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterBankedHelixLargeUpOffsets = {{
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterBankedHelixLargeUpBoundBoxes = {{
        {{
            { { 0, 6, 35 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 43 }, { 32, 16, 3 } },
            { { 0, 16, 43 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 43 }, { 16, 32, 3 } },
            { { 6, 0, 43 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 35 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 43 }, { 16, 32, 3 } },
            { { 16, 16, 43 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 43 }, { 32, 16, 3 } },
            { { 0, 6, 43 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 35 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 43 }, { 32, 16, 3 } },
            { { 16, 0, 43 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 43 }, { 16, 32, 3 } },
            { { 6, 0, 43 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 35 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 43 }, { 16, 32, 3 } },
            { { 0, 0, 43 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 43 }, { 32, 16, 3 } },
            { { 0, 6, 43 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightQuarterBankedHelixLargeUpSprites = {{
        {{
            27089,
            kImageIndexUndefined,
            27090,
            27091,
            kImageIndexUndefined,
            27092,
            27093,
        }},
        {{
            27094,
            kImageIndexUndefined,
            27095,
            27096,
            kImageIndexUndefined,
            27097,
            27098,
        }},
        {{
            27099,
            kImageIndexUndefined,
            27100,
            27101,
            kImageIndexUndefined,
            27102,
            27103,
        }},
        {{
            27104,
            kImageIndexUndefined,
            27105,
            27106,
            kImageIndexUndefined,
            27107,
            27108,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterBankedHelixLargeUpOffsets = {{
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterBankedHelixLargeUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterBankedHelixLargeUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp90Sprites = {{
        {{
            {{
                26605,
                kImageIndexUndefined,
            }},
            {{
                26606,
                kImageIndexUndefined,
            }},
            {{
                26607,
                kImageIndexUndefined,
            }},
            {{
                26608,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                26609,
                kImageIndexUndefined,
            }},
            {{
                26610,
                kImageIndexUndefined,
            }},
            {{
                26611,
                kImageIndexUndefined,
            }},
            {{
                26612,
                kImageIndexUndefined,
            }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToUp90Sprites = {{
        {{
            {{
                26589,
                kImageIndexUndefined,
            }},
            {{
                26590,
                kImageIndexUndefined,
            }},
            {{
                26591,
                kImageIndexUndefined,
            }},
            {{
                26592,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                26597,
                kImageIndexUndefined,
            }},
            {{
                26598,
                kImageIndexUndefined,
            }},
            {{
                26599,
                kImageIndexUndefined,
            }},
            {{
                26600,
                kImageIndexUndefined,
            }},
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kDown90ToDown60Sprites = {{
        {{
            26591,
            26592,
            26589,
            26590,
        }},
        {{
            26599,
            26600,
            26597,
            26598,
        }},
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp90ToUp60Sprites = {{
        {{
            26593,
            26594,
            26595,
            26596,
        }},
        {{
            26601,
            26602,
            26603,
            26604,
        }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kDown60ToDown90Sprites = {{
        {{
            {{
                26595,
                kImageIndexUndefined,
            }},
            {{
                26596,
                kImageIndexUndefined,
            }},
            {{
                26593,
                kImageIndexUndefined,
            }},
            {{
                26594,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                26603,
                kImageIndexUndefined,
            }},
            {{
                26604,
                kImageIndexUndefined,
            }},
            {{
                26601,
                kImageIndexUndefined,
            }},
            {{
                26602,
                kImageIndexUndefined,
            }},
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
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            26765,
            26766,
            26767,
            kImageIndexUndefined,
            26768,
        }},
        {{
            26769,
            26770,
            26771,
            kImageIndexUndefined,
            26772,
        }},
        {{
            26773,
            26774,
            26775,
            kImageIndexUndefined,
            26776,
        }},
        {{
            26777,
            26778,
            26779,
            kImageIndexUndefined,
            26780,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 29 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 0, 0, 29 }, { 16, 34, 3 } },
            { { 16, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 29 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 16, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 0, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 29 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            26749,
            26750,
            26751,
            kImageIndexUndefined,
            26752,
        }},
        {{
            26753,
            26754,
            26755,
            kImageIndexUndefined,
            26756,
        }},
        {{
            26757,
            26758,
            26759,
            kImageIndexUndefined,
            26760,
        }},
        {{
            26761,
            26762,
            26763,
            kImageIndexUndefined,
            26764,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            26853,
            26854,
            26855,
            kImageIndexUndefined,
            26856,
        }},
        {{
            26857,
            26858,
            26859,
            kImageIndexUndefined,
            26860,
        }},
        {{
            26861,
            26862,
            26863,
            kImageIndexUndefined,
            26864,
        }},
        {{
            26865,
            26866,
            26867,
            kImageIndexUndefined,
            26868,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 0, 29 }, { 32, 16, 3 } },
            { { 0, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 29 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 0, 0, 29 }, { 16, 34, 3 } },
            { { 16, 16, 29 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 29 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 16, 29 }, { 32, 16, 3 } },
            { { 16, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 29 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 16, 0, 29 }, { 16, 32, 3 } },
            { { 0, 0, 29 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 29 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            26837,
            26838,
            26839,
            kImageIndexUndefined,
            26840,
        }},
        {{
            26841,
            26842,
            26843,
            kImageIndexUndefined,
            26844,
        }},
        {{
            26845,
            26846,
            26847,
            kImageIndexUndefined,
            26848,
        }},
        {{
            26849,
            26850,
            26851,
            kImageIndexUndefined,
            26852,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            { 0, 0, 29 }, 
            {},
            { 0, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                26781,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26782,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26783,
                kImageIndexUndefined,
            }},
            {{
                26784,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26809,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26810,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26811,
                kImageIndexUndefined,
            }},
            {{
                26812,
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
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 3 } },
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
                26793,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26794,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26795,
                kImageIndexUndefined,
            }},
            {{
                26796,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26821,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26822,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26823,
                kImageIndexUndefined,
            }},
            {{
                26824,
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
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 45 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 45 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 45 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 45 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            kImageIndexUndefined,
            26805,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26806,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26807,
            kImageIndexUndefined,
        }},
        {{
            26808,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60Offsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60BoundBoxes = {{
        {{
            {},
            { { -16, -16, 93 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 93 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 93 }, { 32, 32, 3 } },
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
                26785,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26786,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26787,
                kImageIndexUndefined,
            }},
            {{
                26788,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26813,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26814,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26815,
                kImageIndexUndefined,
            }},
            {{
                26816,
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
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            kImageIndexUndefined,
            26797,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26798,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26799,
            kImageIndexUndefined,
        }},
        {{
            26800,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Offsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60BoundBoxes = {{
        {{
            {},
            { { -16, -16, 61 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 61 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 61 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 61 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            26801,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26802,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26803,
            kImageIndexUndefined,
        }},
        {{
            26804,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Offsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 61 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { 0, 0, 61 }, { 16, 16, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 61 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 61 }, { 32, 32, 3 } },
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
                26789,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26790,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26791,
                kImageIndexUndefined,
            }},
            {{
                26792,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26817,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26818,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26819,
                kImageIndexUndefined,
            }},
            {{
                26820,
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
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            26873,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26874,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26875,
            kImageIndexUndefined,
        }},
        {{
            26876,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankOffsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            kImageIndexUndefined,
            26877,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26878,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26879,
            kImageIndexUndefined,
        }},
        {{
            26880,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankOffsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            26889,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26890,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26891,
            kImageIndexUndefined,
        }},
        {{
            26892,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Offsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            26893,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26894,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26895,
            kImageIndexUndefined,
        }},
        {{
            26896,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Offsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            26881,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26882,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26883,
            kImageIndexUndefined,
        }},
        {{
            26884,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankOffsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            kImageIndexUndefined,
            26885,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26886,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26887,
            kImageIndexUndefined,
        }},
        {{
            26888,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankOffsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 37 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            26869,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26870,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26871,
            kImageIndexUndefined,
        }},
        {{
            26872,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankOffsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBlockBrakesSprites = {{
        26559,
        26560,
        26559,
        26560,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBlockBrakesOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
        { { 0, 6, 29 }, { 32, 20, 3 } },
        { { 6, 0, 29 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SLC_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SLC_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SLC_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SLC_DIAG_BRAKES + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesOffsets = {{
        {{
            {},
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 3 } },
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
                SPR_G2_SLC_DIAG_BRAKES + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SLC_DIAG_BRAKES + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SLC_DIAG_BRAKES + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SLC_DIAG_BRAKES + 1,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SLC_DIAG_BRAKES + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SLC_DIAG_BRAKES + 2,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SLC_DIAG_BRAKES + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SLC_DIAG_BRAKES + 2,
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
            { -16, -16, 29 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 29 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 29 }, 
            {},
        }},
        {{
            { -16, -16, 29 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0][0], &kUp25Offsets[0], &kUp25BoundBoxes[0], false }, // Up25
        { &kUp60Sprites[0][0], &kUp60Offsets[0], &kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0][0], &kUp25ToUp60Offsets[0][0], &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0][0], &kUp60ToUp25Offsets[0][0], &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], &kUp25ToFlatOffsets[0], &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0], &kLeftQuarterTurn5TilesOffsets[0][0], &kLeftQuarterTurn5TilesBoundBoxes[0][0], false }, // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0], &kFlatToLeftBankOffsets[0], &kFlatToLeftBankBoundBoxes[0], false }, // FlatToLeftBank
        { &kFlatToRightBankSprites[0], &kFlatToRightBankOffsets[0], &kFlatToRightBankBoundBoxes[0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { &kBankedLeftQuarterTurn5TilesSprites[0][0], &kBankedLeftQuarterTurn5TilesOffsets[0][0], &kBankedLeftQuarterTurn5TilesBoundBoxes[0][0], false }, // BankedLeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // BankedRightQuarterTurn5Tiles
        { &kLeftBankToUp25Sprites[0], &kLeftBankToUp25Offsets[0], &kLeftBankToUp25BoundBoxes[0], false }, // LeftBankToUp25
        { &kRightBankToUp25Sprites[0], &kRightBankToUp25Offsets[0], &kRightBankToUp25BoundBoxes[0], false }, // RightBankToUp25
        { &kUp25ToLeftBankSprites[0], &kUp25ToLeftBankOffsets[0], &kUp25ToLeftBankBoundBoxes[0], false }, // Up25ToLeftBank
        { &kUp25ToRightBankSprites[0], &kUp25ToRightBankOffsets[0], &kUp25ToRightBankBoundBoxes[0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBank
        { &kLeftBankSprites[0], &kLeftBankOffsets[0], &kLeftBankBoundBoxes[0], false }, // LeftBank
        { nullptr, nullptr, nullptr, true }, // RightBank
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0], &kLeftQuarterTurn5TilesUp25Offsets[0][0], &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0], &kRightQuarterTurn5TilesUp25Offsets[0][0], &kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], &kSBendLeftOffsets[0][0], &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], &kSBendRightOffsets[0][0], &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { &kLeftVerticalLoopSprites[0][0], &kLeftVerticalLoopOffsets[0][0], &kLeftVerticalLoopBoundBoxes[0][0], false }, // LeftVerticalLoop
        { &kRightVerticalLoopSprites[0][0], &kRightVerticalLoopOffsets[0][0], &kRightVerticalLoopBoundBoxes[0][0], false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], &kLeftQuarterTurn3TilesOffsets[0][0], &kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0], &kLeftBankedQuarterTurn3TilesOffsets[0][0], &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &kLeftQuarterTurn3TilesUp25Sprites[0][0], &kLeftQuarterTurn3TilesUp25Offsets[0][0], &kLeftQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0], &kRightQuarterTurn3TilesUp25Offsets[0][0], &kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1Tile
        { &kLeftTwistDownToUpSprites[0][0], &kLeftTwistDownToUpOffsets[0][0], &kLeftTwistDownToUpBoundBoxes[0][0], false }, // LeftTwistDownToUp
        { &kRightTwistDownToUpSprites[0][0], &kRightTwistDownToUpOffsets[0][0], &kRightTwistDownToUpBoundBoxes[0][0], false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true }, // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true }, // RightTwistUpToDown
        { &kHalfLoopUpSprites[0][0], &kHalfLoopUpOffsets[0][0], &kHalfLoopUpBoundBoxes[0][0], false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true }, // HalfLoopDown
        { &kLeftCorkscrewUpSprites[0][0], &kLeftCorkscrewUpOffsets[0][0], &kLeftCorkscrewUpBoundBoxes[0][0], false }, // LeftCorkscrewUp
        { &kRightCorkscrewUpSprites[0][0], &kRightCorkscrewUpOffsets[0][0], &kRightCorkscrewUpBoundBoxes[0][0], false }, // RightCorkscrewUp
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
        { &kLeftQuarterTurn1TileUp60Sprites[0][0], &kLeftQuarterTurn1TileUp60Offsets[0][0], &kLeftQuarterTurn1TileUp60BoundBoxes[0][0], false }, // LeftQuarterTurn1TileUp60
        { &kRightQuarterTurn1TileUp60Sprites[0][0], &kRightQuarterTurn1TileUp60Offsets[0][0], &kRightQuarterTurn1TileUp60BoundBoxes[0][0], false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0], &kBrakesOffsets[0], &kBrakesBoundBoxes[0], false }, // Brakes
        { nullptr, nullptr, nullptr, false }, // Booster
        { nullptr, nullptr, nullptr, false }, // Maze
        { &kLeftQuarterBankedHelixLargeUpSprites[0][0], &kLeftQuarterBankedHelixLargeUpOffsets[0][0], &kLeftQuarterBankedHelixLargeUpBoundBoxes[0][0], false }, // LeftQuarterBankedHelixLargeUp
        { &kRightQuarterBankedHelixLargeUpSprites[0][0], &kRightQuarterBankedHelixLargeUpOffsets[0][0], &kRightQuarterBankedHelixLargeUpBoundBoxes[0][0], false }, // RightQuarterBankedHelixLargeUp
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
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // OnRidePhoto
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
        { &kUp90Sprites[0][0][0], &kUp90Offsets[0][0], &kUp90BoundBoxes[0][0], false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { &kUp60ToUp90Sprites[0][0][0], &kUp60ToUp90Offsets[0][0], &kUp60ToUp90BoundBoxes[0][0], false }, // Up60ToUp90
        { &kDown90ToDown60Sprites[0][0], &kDown90ToDown60Offsets[0], &kDown90ToDown60BoundBoxes[0], false }, // Down90ToDown60
        { &kUp90ToUp60Sprites[0][0], &kUp90ToUp60Offsets[0], &kUp90ToUp60BoundBoxes[0], false }, // Up90ToUp60
        { &kDown60ToDown90Sprites[0][0][0], &kDown60ToDown90Offsets[0][0], &kDown60ToDown90BoundBoxes[0][0], false }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &kLeftEighthToDiagSprites[0][0], &kLeftEighthToDiagOffsets[0][0], &kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0], &kRightEighthToDiagOffsets[0][0], &kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { &kLeftEighthBankToDiagSprites[0][0], &kLeftEighthBankToDiagOffsets[0][0], &kLeftEighthBankToDiagBoundBoxes[0][0], false }, // LeftEighthBankToDiag
        { &kRightEighthBankToDiagSprites[0][0], &kRightEighthBankToDiagOffsets[0][0], &kRightEighthBankToDiagBoundBoxes[0][0], false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0], &kDiagFlatOffsets[0][0], &kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0], &kDiagUp25Offsets[0][0], &kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { &kDiagUp60Sprites[0][0], &kDiagUp60Offsets[0][0], &kDiagUp60BoundBoxes[0][0], false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0], &kDiagUp25ToUp60Offsets[0][0], &kDiagUp25ToUp60BoundBoxes[0][0], false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0], &kDiagUp60ToUp25Offsets[0][0], &kDiagUp60ToUp25BoundBoxes[0][0], false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0], &kDiagUp25ToFlatOffsets[0][0], &kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
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
        { &kDiagFlatToLeftBankSprites[0][0], &kDiagFlatToLeftBankOffsets[0][0], &kDiagFlatToLeftBankBoundBoxes[0][0], false }, // DiagFlatToLeftBank
        { &kDiagFlatToRightBankSprites[0][0], &kDiagFlatToRightBankOffsets[0][0], &kDiagFlatToRightBankBoundBoxes[0][0], false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToFlat
        { &kDiagLeftBankToUp25Sprites[0][0], &kDiagLeftBankToUp25Offsets[0][0], &kDiagLeftBankToUp25BoundBoxes[0][0], false }, // DiagLeftBankToUp25
        { &kDiagRightBankToUp25Sprites[0][0], &kDiagRightBankToUp25Offsets[0][0], &kDiagRightBankToUp25BoundBoxes[0][0], false }, // DiagRightBankToUp25
        { &kDiagUp25ToLeftBankSprites[0][0], &kDiagUp25ToLeftBankOffsets[0][0], &kDiagUp25ToLeftBankBoundBoxes[0][0], false }, // DiagUp25ToLeftBank
        { &kDiagUp25ToRightBankSprites[0][0], &kDiagUp25ToRightBankOffsets[0][0], &kDiagUp25ToRightBankBoundBoxes[0][0], false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBank
        { &kDiagLeftBankSprites[0][0], &kDiagLeftBankOffsets[0][0], &kDiagLeftBankBoundBoxes[0][0], false }, // DiagLeftBank
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
        { &kBlockBrakesSprites[0], &kBlockBrakesOffsets[0], &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
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
        { &kDiagBrakesSprites[0][0], &kDiagBrakesOffsets[0][0], &kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0], &kDiagBlockBrakesOffsets[0][0], &kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
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
} // namespace OpenRCT2::CompactInvertedCoaster
