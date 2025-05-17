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

namespace OpenRCT2::SingleRailRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT + 0,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT + 1,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT + 0,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT + 1,
        }},
        {{
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT + 0,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT + 1,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT + 2,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT + 3,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 8,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 9,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 10,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 8,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 9,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 10,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 11,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp60Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_STEEP + 12,
            SPR_G2_SINGLE_RAIL_TRACK_STEEP + 13,
            SPR_G2_SINGLE_RAIL_TRACK_STEEP + 14,
            SPR_G2_SINGLE_RAIL_TRACK_STEEP + 15,
        }},
        {{
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 12,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 13,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 14,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 15,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 27, 0, 0 }, { 1, 32, 98 } },
        { { 0, 27, 0 }, { 32, 1, 98 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 0,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 1,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 2,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 0,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 1,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 2,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 3,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToUp60Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 1, SPR_G2_SINGLE_RAIL_TRACK_STEEP + 2, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 3, SPR_G2_SINGLE_RAIL_TRACK_STEEP + 4, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 1, SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 2, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 3, SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 4, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 5, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToUp25Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 7, SPR_G2_SINGLE_RAIL_TRACK_STEEP + 8, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 9, SPR_G2_SINGLE_RAIL_TRACK_STEEP + 10, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP + 11, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 7, SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 8, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 9, SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 10, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP + 11, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 4,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 5,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 6,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 4,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 5,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 6,
            SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE + 7,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 0,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 1,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 3,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 4,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 5,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 6,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 7,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 8,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 11,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 12,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 13,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 14,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 15,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 16,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 17,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 18,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE + 19,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 0, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 1, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 2, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 3, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 4, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 5, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 7, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 8, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 9, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 10, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 11, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 0, SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 4, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 7, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 9, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 14, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 15, SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 16, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 18, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 20, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_CURVE_BANKED + 21, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 12, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 13, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 14, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 15, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 16, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 17, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 18, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 19, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 20, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 21, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 22, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 23, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 24, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 25, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 26, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 27, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 28, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 29, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 30, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 31, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 32, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 33, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 34, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 35, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 36,
        SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 37,
        SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 38,
        SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION + 39,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        { { 0, 27, 0 }, { 32, 1, 26 } },
        { { 27, 0, 0 }, { 1, 32, 26 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 0,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 1,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 3,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 4,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 5,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 6,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 7,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 8,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 11,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 12,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 13,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 14,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 15,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 16,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 17,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 18,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 19,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 20,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 21,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 23,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 24,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 25,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 26,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 27,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 28,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 29,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 31,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 32,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 33,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 34,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 35,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 36,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 38,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 0,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 1,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 2,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 4,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 5,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 6,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 3,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 2,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 1,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 0,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 7,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 6,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 5,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 4,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 0 }, { 32, 26, 3 } },
            { { 0, 6, 0 }, { 32, 26, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 0, 0, 0 }, { 26, 32, 3 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 26, 3 } },
            { { 0, 0, 0 }, { 32, 26, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 0, 0, 0 }, { 26, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 8,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 9,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 10,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 12,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 13,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 14,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 15,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 11,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 10,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 9,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 8,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 15,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 14,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 13,
            SPR_G2_SINGLE_RAIL_TRACK_S_BEND + 12,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 0,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 1,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 2,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 3,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 4,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 5,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 7,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 8,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 9,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 10,
            SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE + 11,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 16, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 0, SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 8, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 9, SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 10, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_CURVE_BANKED + 13, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 1,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 5,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 7,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 8,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 13,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE + 15,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn3TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kHalfLoopUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 0,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 2,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 4,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 5,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 6,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 8,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 9,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 12,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 13,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 14,
            SPR_G2_SINGLE_RAIL_TRACK_HALF_LOOP + 15,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kHalfLoopUpOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 16, 0, 0 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 14, 0, 0 }, 
            { 0, 12, 0 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 10, 16, 0 }, 
            { 0, 16, 32 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 16, 16, 0 }, 
            { 16, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kHalfLoopUpBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 16, 0, 0 }, { 2, 16, 119 } },
            { { 0, 0, 32 }, { 32, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 11 } },
            { { 6, 28, 0 }, { 20, 3, 63 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 0, 0, 32 }, { 16, 32, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 9 } },
            { { 28, 6, 0 }, { 3, 20, 63 } },
            { { 10, 16, 0 }, { 4, 12, 119 } },
            { { 0, 16, 32 }, { 32, 12, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 7 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 15, 0 }, { 16, 2, 119 } },
            { { 16, 0, 32 }, { 12, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftCorkscrewUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 0,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 1,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 2,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 3,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 4,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 5,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 6,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 7,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 8,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 9,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 10,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 11,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftCorkscrewUpBoundBoxes = {{
        {{
            { { 0, 6, 4 }, { 32, 20, 3 } },
            { { 6, 6, 10 }, { 20, 20, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 4 }, { 20, 32, 3 } },
            { { 6, 6, 10 }, { 20, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 4 }, { 32, 20, 3 } },
            { { 6, 6, 10 }, { 20, 20, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 4 }, { 20, 32, 3 } },
            { { 6, 6, 10 }, { 20, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 12,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 13,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 14,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 15,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 16,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 17,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 18,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 19,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 20,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 21,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 22,
            SPR_G2_SINGLE_RAIL_TRACK_CORKSCREW + 23,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatToUp60Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 2, SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 1, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 4, SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 3, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 2, SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 1, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 4, SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 3, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 4 } }, {} }},
        {{ { { 4, 29, 2 }, { 24, 1, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 29, 4, 2 }, { 1, 24, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 4 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToFlatSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 7, SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 8, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 10, SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 9, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 11, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 7, SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 8, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 10, SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 9, }},
            {{ SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 11, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToFlatBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 4 } }, {} }},
        {{ { { 4, 0, 0 }, { 2, 32, 43 } }, { { 4, 29, 2 }, { 24, 1, 43 } } }},
        {{ { { 29, 4, 2 }, { 1, 24, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 4 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 0, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 3, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 13, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 5, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 0, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 8, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 9, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 10, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 5, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 13, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 8, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 9, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 10, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 15, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 16, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 19, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 20, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 19, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 20, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 21, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 22, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 23, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 21, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 22, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 23, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 24, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 25, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 26, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 27, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 24, SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 25, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 26, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 27, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 15, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_SMALL_HELIX + 16, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 0, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 4, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 5, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 20, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 21, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 7, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 9, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 10, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 0, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 4, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 14, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 15, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 16, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 7, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 9, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 20, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 21, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 14, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 15, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 16, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 22, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 23, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 24, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 25, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 26, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 27, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 28, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 29, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 30, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 31, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 32, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 27, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 28, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 29, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 30, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 31, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 32, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 33, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 34, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 35, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 36, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 37, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 33, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 34, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 35, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 36, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 37, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 38, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 39, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 40, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 41, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 42, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 43, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 38, SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 39, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 40, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 41, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 42, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 43, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 22, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 23, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 24, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 25, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_HELIX + 26, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 0, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 1, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 2, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 3, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 4, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 5, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 6, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 7, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60BoundBoxes = {{
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 8, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 9, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 10, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 11, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 12, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 13, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 14, SPR_G2_SINGLE_RAIL_TRACK_STEEP_SMALL_CURVE + 15, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_BRAKE + 0,
        SPR_G2_SINGLE_RAIL_TRACK_BRAKE + 1,
        SPR_G2_SINGLE_RAIL_TRACK_BRAKE + 0,
        SPR_G2_SINGLE_RAIL_TRACK_BRAKE + 1,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25LeftBankedSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 36,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 37,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 38,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 39,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25LeftBankedBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 27, 0, 0 }, { 1, 32, 34 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25RightBankedSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 40,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 41,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 42,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 43,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 0,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 2,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 4,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 5,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 6,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 8,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 9,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 12,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 13,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 14,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 15,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 27, 0, 0 }, { 1, 32, 98 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 27, 0 }, { 32, 1, 98 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 16,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 17,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 18,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 19,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 20,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 21,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 22,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 23,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 24,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 25,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 26,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 27,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 28,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 29,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 30,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 27, 0, 0 }, { 1, 32, 98 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 27, 0 }, { 32, 1, 98 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp90Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 9,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 10,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 11,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp90BoundBoxes = {{
        {{
            { { 4, 6, 8 }, { 2, 20, 31 } },
            {},
        }},
        {{
            { { 6, 24, 8 }, { 20, 2, 31 } },
            {},
        }},
        {{
            { { 24, 6, 8 }, { 2, 20, 31 } },
            {},
        }},
        {{
            { { 6, 4, 8 }, { 20, 2, 31 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp90Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 1,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 2,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 3,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp90BoundBoxes = {{
        {{
            { { 4, 6, 8 }, { 13, 20, 55 } },
            {},
        }},
        {{
            { { 6, 24, 0 }, { 20, 2, 55 } },
            {},
        }},
        {{
            { { 24, 6, 0 }, { 2, 20, 55 } },
            {},
        }},
        {{
            { { 6, 4, 8 }, { 20, 13, 55 } },
            {},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kDown90ToDown60Sprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 2,
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 3,
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 0,
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 1,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kDown90ToDown60BoundBoxes = {{
        { { 24, 6, 0 }, { 2, 20, 55 } },
        { { 6, 4, 8 }, { 20, 13, 55 } },
        { { 4, 6, 8 }, { 13, 20, 55 } },
        { { 6, 24, 0 }, { 20, 2, 55 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp90ToUp60Sprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 4,
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 5,
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 6,
        SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 7,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp90ToUp60BoundBoxes = {{
        { { 4, 6, 8 }, { 2, 20, 48 } },
        { { 6, 24, 8 }, { 20, 2, 31 } },
        { { 24, 6, 8 }, { 2, 20, 31 } },
        { { 6, 4, 8 }, { 20, 2, 48 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 6,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 7,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 4,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_VERTICAL + 5,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90BoundBoxes = {{
        {{
            { { 24, 6, 8 }, { 2, 20, 31 } },
            {},
        }},
        {{
            { { 6, 4, 8 }, { 20, 2, 48 } },
            {},
        }},
        {{
            { { 4, 6, 8 }, { 2, 20, 48 } },
            {},
        }},
        {{
            { { 6, 24, 8 }, { 20, 2, 31 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 0,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 1,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 4,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 5,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 8,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 9,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 12,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 13,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 15,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 0, 0, 0 }, { 16, 34, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 16,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 17,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 20,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 21,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 24,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 25,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 28,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 29,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 0,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 1,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 4,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 5,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 6,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 8,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 9,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 10,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 12,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 13,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 14,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 15,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagBoundBoxes = {{
        {{
            { { 0, 27, 0 }, { 32, 1, 26 } },
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 27, 0, 0 }, { 1, 32, 26 } },
            { { 0, 0, 27 }, { 16, 34, 0 } },
            { { 16, 16, 27 }, { 16, 16, 0 } },
            {},
            { { 16, 0, 27 }, { 18, 16, 0 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 16,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 17,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 18,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 19,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 20,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 21,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 23,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 24,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 25,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 27,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 28,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 29,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CURVE_BANKED + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_FLAT_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_FLAT_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_FLAT_DIAGONAL + 0,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_FLAT_DIAGONAL + 1,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_FLAT_DIAGONAL + 3,
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
    kDiagUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 8,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 9,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 10,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 11,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 8,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 9,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 10,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 11,
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
    kDiagUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 8,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 9,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 10,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 11,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 8,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 9,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 10,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 11,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60Offsets = {{
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
    kDiagUp60BoundBoxes = {{
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
    kDiagFlatToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 3,
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
    kDiagUp25ToUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Offsets = {{
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
    kDiagUp25ToUp60BoundBoxes = {{
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
    kDiagUp60ToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 6,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_STEEP_DIAGONAL + 7,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 6,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_STEEP_DIAGONAL + 7,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Offsets = {{
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
    kDiagUp60ToUp25BoundBoxes = {{
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
            { { 0, 0, 0 }, { 16, 16, 3 } },
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
    kDiagUp25ToFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 6,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_GENTLE_DIAGONAL + 7,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 6,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_GENTLE_DIAGONAL + 7,
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
    kDiagFlatToUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 12,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 13,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 14,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 15,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 12,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 13,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 14,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 15,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp60Offsets = {{
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
    kDiagFlatToUp60BoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 4 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 4 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 4 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 4 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp60ToFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 16,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 17,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 18,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_TRACK_SMALL_FLAT_TO_STEEP + 19,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 16,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 17,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 18,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_SINGLE_RAIL_LIFT_TRACK_SMALL_FLAT_TO_STEEP + 19,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60ToFlatOffsets = {{
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
    kDiagUp60ToFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 4 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 4 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 4 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 4 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 0, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankOffsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 7, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankOffsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 10, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 17, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 20, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 21, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 22, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 23, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 24, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankOffsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 25, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 26, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 27, SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 28, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 29, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankOffsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 30,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 31,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 32,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_BANK_TRANSITION_DIAGONAL + 33,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankOffsets = {{
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
    kDiagLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 27 }, { 32, 32, 0 } },
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftBarrelRollUpToDownSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 0, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 1, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 2, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 3, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 4, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 5, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 6, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 7, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 8, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 9, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 10, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 12, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 13, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 14, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 15, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 16, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 17, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 18, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 19, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 20, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 21, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 22, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 23, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftBarrelRollUpToDownBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 28 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 28 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 44 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 28 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 28 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 44 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 28 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 28 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 44 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 28 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 28 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 44 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 24, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 25, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 26, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 27, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 28, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 29, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 30, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 31, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 32, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 33, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 34, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 35, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 36, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 37, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 38, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 39, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 40, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 41, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 42, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 43, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 44, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 45, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 46, SPR_G2_SINGLE_RAIL_TRACK_BARREL_ROLL + 47, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBarrelRollUpToDownBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 1, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 2, SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 8, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Offsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, { 6, 0, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 6, -6 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 6, 0, -6 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 6, -6 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 6, 0, -6 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 13, SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 14, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 16, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_TURN_BANK_TRANSITION + 17, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Offsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
        {{
            {{ { 0, 6, 0 }, { 0, 6, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 0,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 2,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 3,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 4,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 5,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 6,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 7,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 8,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 9,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 11,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 12,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 13,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 14,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 15,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 16,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 17,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 18,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 19,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 20,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 21,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 22,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 23,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 24,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 25,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 26,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 27,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 16, 32 }, { 32, 16, 0 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 9 } },
            { { 0, 0, 70 }, { 16, 32, 0 } },
            { { 0, 0, 200 }, { 16, 32, 0 } },
            { { 16, 0, 110 }, { 16, 16, 0 } },
            { { 16, 0, 200 }, { 16, 32, 0 } },
            { { 16, 0, 32 }, { 16, 32, 0 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 16, 70 }, { 32, 16, 0 } },
            { { 0, 16, 200 }, { 32, 16, 0 } },
            { { 0, 0, 100 }, { 16, 16, 0 } },
            { { 0, 0, 200 }, { 32, 16, 0 } },
            { { 0, 0, 32 }, { 32, 16, 0 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 32 }, { 16, 32, 0 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 28,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 29,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 30,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 31,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 32,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 33,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 34,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 35,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 36,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 37,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 38,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 39,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 40,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 41,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 42,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 43,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 44,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 45,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 46,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 47,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 48,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 49,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 50,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 51,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 52,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 53,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 54,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_HALF_LOOP + 55,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 0,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 2,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 3,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 4,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 5,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 6,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 7,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 8,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 9,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 11,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopBoundBoxes = {{
        {{
            { { 4, 6, 8 }, { 2, 20, 31 } },
            { { -8, 6, 0 }, { 2, 20, 31 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 24, 8 }, { 20, 2, 31 } },
            { { 6, 24, 8 }, { 20, 2, 63 } },
            { { 6, 24, 8 }, { 20, 2, 31 } },
        }},
        {{
            { { 24, 6, 8 }, { 2, 20, 31 } },
            { { 24, 6, 8 }, { 2, 20, 63 } },
            { { 24, 6, 8 }, { 2, 20, 31 } },
        }},
        {{
            { { 6, 4, 8 }, { 20, 2, 31 } },
            { { 6, -8, 0 }, { 20, 2, 31 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kInvertedFlatToDown90QuarterLoopSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 2,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 5,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 4,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 3,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 8,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 7,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 6,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 11,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_QUARTER_LOOP + 9,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kInvertedFlatToDown90QuarterLoopBoundBoxes = {{
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { -8, 6, 0 }, { 2, 20, 31 } },
            { { 4, 6, 8 }, { 2, 20, 31 } },
            {},
        }},
        {{
            { { 6, 24, 8 }, { 20, 2, 31 } },
            { { 6, 24, 8 }, { 20, 2, 63 } },
            { { 6, 24, 8 }, { 20, 2, 31 } },
            {},
        }},
        {{
            { { 24, 6, 8 }, { 2, 20, 31 } },
            { { 24, 6, 8 }, { 2, 20, 63 } },
            { { 24, 6, 8 }, { 2, 20, 31 } },
            {},
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, -8, 0 }, { 20, 2, 31 } },
            { { 6, 4, 8 }, { 20, 2, 31 } },
            {},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBlockBrakesSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE + 0,
        SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE + 1,
        SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE + 0,
        SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE + 1,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 1,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 5,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 7,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 27, 0, 0 }, { 1, 32, 34 } },
            {},
            {},
            { { 0, 27, 0 }, { 32, 1, 34 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            {},
            { { 27, 0, 0 }, { 1, 32, 34 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 8,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 13,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_SMALL_CURVE_BANKED + 15,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            {},
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            {},
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn3TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn5TileUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 0,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 1,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 3,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 4,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 5,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 6,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 7,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 8,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 11,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 12,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 13,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 14,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 15,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 16,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 17,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 18,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 19,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn5TileUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 27, 0, 0 }, { 1, 32, 34 } },
            {},
            { { 30, 30, 0 }, { 1, 1, 34 } },
            { { 30, 30, 0 }, { 1, 1, 34 } },
            {},
            { { 30, 30, 0 }, { 1, 1, 34 } },
            { { 0, 27, 0 }, { 32, 1, 34 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 27, 0, 0 }, { 1, 32, 34 } },
            { { 27, 0, 0 }, { 1, 32, 34 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 20,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 21,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 22,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 23,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 24,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 25,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 26,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 27,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 28,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 29,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 30,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 31,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 32,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 33,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 34,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 35,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 36,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 37,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 38,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 0, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 1, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 2, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 3, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 4, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 5, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 7, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 8, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 9, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 11, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 12, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 13, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 14, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 16, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 17, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 18, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 19, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 20,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 21,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 22,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 23,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25BoundBoxes = {{
        { { 0, 27, 0 }, { 32, 1, 34 } },
        { { 27, 0, 0 }, { 1, 32, 34 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25Sprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 24,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 25,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 26,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 27,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 28,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 29,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 30,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 31,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatBoundBoxes = {{
        { { 0, 27, 0 }, { 32, 1, 34 } },
        { { 27, 0, 0 }, { 1, 32, 34 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatSprites = {{
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 32,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 33,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 34,
        SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 35,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 44, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 45, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 46, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 47, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 48, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25Sprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 49, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 50, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 51, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 52, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 53, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 54, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 55, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 56, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 57, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 58, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatSprites = {{
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 59, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 60, kImageIndexUndefined, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 61, SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 62, }},
        {{ SPR_G2_SINGLE_RAIL_TRACK_SLOPE_BANK_TRANSITION + 63, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 1, SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 2, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 4, SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 5, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90BoundBoxes = {{
        {{
            {{ { { 4, 6, 8 }, { 2, 20, 63 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 63 } }, { { 6, 24, 8 }, { 20, 2, 63 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 24, 6, 8 }, { 2, 20, 63 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 63 } }, { { 6, 24, 8 }, { 20, 2, 63 } } }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90Sprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 6, SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 7, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 9, SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 10, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_VERTICAL_TWIST + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 0,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 1,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 3,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 4,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 5,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 6,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 7,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 8,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 10,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 11,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 12,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 13,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 14,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 15,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 16,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 17,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 18,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 19,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpBoundBoxes = {{
        {{
            { { 0, 6, 4 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 31, 0 }, { 48, 1, 64 } },
            {},
            { { 2, 2, 50 }, { 28, 28, 0 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
        }},
        {{
            { { 6, 0, 4 }, { 20, 32, 3 } },
            { { 29, 0, 0 }, { 1, 26, 32 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
            {},
            { { 2, 2, 50 }, { 28, 28, 0 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
        }},
        {{
            { { 0, 6, 4 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 20, 20, 3 } },
            { { 2, 2, 48 }, { 28, 28, 1 } },
            {},
            { { 2, 2, 50 }, { 24, 28, 0 } },
            { { 2, 2, 40 }, { 26, 28, 1 } },
        }},
        {{
            { { 6, 0, 4 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 2, 2, 40 }, { 28, 26, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 20,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 21,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 22,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 23,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 24,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 25,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 26,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 27,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 28,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 29,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 30,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 31,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 32,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 33,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 34,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 35,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 36,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 37,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 38,
            SPR_G2_SINGLE_RAIL_TRACK_LARGE_CORKSCREW + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 0,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 2,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 3,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 4,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 5,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 6,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 7,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 8,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 9,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 11,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 12,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 13,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 14,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 15,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 16,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 17,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 18,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 19,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 2 }, { 32, 32, 0 } },
            { { 2, 0, 0 }, { 1, 32, 160 } },
            { { 0, 2, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 31, 0, 0 }, { 32, 0, 64 } },
            { { 0, 29, 0 }, { 32, 1, 96 } },
            { { 0, 30, 0 }, { 32, 0, 160 } },
            { { 2, 0, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 32, 0 }, { 40, 0, 64 } },
            { { 31, 0, 0 }, { 1, 32, 96 } },
            { { 29, 0, 0 }, { 1, 32, 160 } },
            { { 0, 0, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 0, 0, 0 }, { 32, 32, 0 } },
            { { 0, 0, 0 }, { 32, 1, 160 } },
            { { 0, 0, 48 }, { 32, 32, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpSprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 20,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 21,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 22,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 23,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 24,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 25,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 26,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 27,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 28,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 29,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 30,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 31,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 32,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 33,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 34,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 35,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 36,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 37,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 38,
            SPR_G2_SINGLE_RAIL_TRACK_MEDIUM_HALF_LOOP + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 0, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 1, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 2, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 4, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 5, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 9, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 10, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 14, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 15, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 28 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 28 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 28 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 31, 0, 0 }, { 1, 32, 32 } } }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 33 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 33 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 17, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 18, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 19, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 22, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 23, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 24, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 25, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 26, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 28, SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 29, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 0, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 1, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 2, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 5, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 9, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 10, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 11, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 13, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 14, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 15, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 17, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 18, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 19, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 96 } } }},
            {{ { { 0, 30, 0 }, { 32, 0, 64 } }, {} }},
            {{ { { 0, 26, 0 }, { 26, 0, 20 } }, {} }},
        }},
        {{
            {{ { { 30, 0, 0 }, { 0, 32, 96 } }, {} }},
            {{ { { 30, 0, 0 }, { 0, 32, 64 } }, {} }},
            {{ { { 30, 0, 0 }, { 0, 32, 48 } }, {} }},
            {{ { { 26, 0, 0 }, { 0, 26, 20 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 96 } } }},
            {{ { { 0, 2, 0 }, { 32, 0, 64 } }, {} }},
            {{ { { 0, 2, 0 }, { 32, 0, 48 } }, {} }},
            {{ { { 0, 2, 0 }, { 32, 0, 32 } }, { { 0, 6, 40 }, { 32, 20, 3 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 2, 0, 0 }, { 0, 32, 32 } }, { { 6, 0, 40 }, { 20, 32, 3 } } }},
            {{ { { 18, 0, 0 }, { 10, 32, 20 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 22, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 23, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 25, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 26, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 28, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 29, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 30, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 32, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 33, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 35, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 36, SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 37, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 38, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_LARGE_ZERO_G_ROLL + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 0,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 1,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 4,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 5,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 8,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 9,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 12,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 13,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 15,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 0, 0, 0 }, { 16, 34, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 16,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 17,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 20,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 21,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 24,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 25,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 28,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 29,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 32,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 33,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 34,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 35,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 36,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 38,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 39,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 40,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 41,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 42,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 43,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 44,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 45,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 46,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 47,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 16, 16, 0 }, { 18, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 34, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 16, 0, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 48,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 49,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 50,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 51,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 52,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 53,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 54,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 55,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 56,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 57,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 58,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 59,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 60,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 61,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 62,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE + 63,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 16, 16, 0 }, { 16, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 16, 0, 0 }, { 16, 18, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankedUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 0, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankedUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankedUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 48 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankedUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 7, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankedUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankedUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 48 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 10, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 48 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 17, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 48 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedFlatToLeftBankedUp25Sprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 21,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 23,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedFlatToLeftBankedUp25Offsets = {{
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
    kDiagLeftBankedFlatToLeftBankedUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 27 }, { 32, 32, 0 } },
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
    kDiagRightBankedFlatToRightBankedUp25Sprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 24,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 25,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 27,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankedFlatToRightBankedUp25Offsets = {{
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
    kDiagRightBankedFlatToRightBankedUp25BoundBoxes = {{
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
            { { -16, -16, 27 }, { 32, 32, 0 } },
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
    kDiagLeftBankedUp25ToLeftBankedFlatSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 29,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 31,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToLeftBankedFlatOffsets = {{
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
    kDiagLeftBankedUp25ToLeftBankedFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 27 }, { 32, 32, 0 } },
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
    kDiagRightBankedUp25ToRightBankedFlatSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 32,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 33,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 34,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 35,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToRightBankedFlatOffsets = {{
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
    kDiagRightBankedUp25ToRightBankedFlatBoundBoxes = {{
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
            { { -16, -16, 27 }, { 32, 32, 0 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankedUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 44, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 46, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 47, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 48, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankedUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankedUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankedUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 49, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 50, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 51, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 52, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 53, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankedUp25Offsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankedUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToFlatSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 54, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 55, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 56, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 57, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 58, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToFlatOffsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankedUp25ToFlatBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToFlatSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 59, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 60, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 61, SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 62, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 63, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToFlatOffsets = {{
        {{
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToFlatBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25LeftBankedSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 36,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 37,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 38,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 39,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25LeftBankedOffsets = {{
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
    kDiagUp25LeftBankedBoundBoxes = {{
        {{
            {},
            { { -16, -16, 48 }, { 32, 32, 0 } },
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
    kDiagUp25RightBankedSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 40,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 41,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 42,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 43,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25RightBankedOffsets = {{
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
    kDiagUp25RightBankedBoundBoxes = {{
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
            { { -16, -16, 48 }, { 32, 32, 0 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 64,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 65,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 66,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 67,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 68,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 69,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 70,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 71,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 72,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 73,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 74,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 75,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 76,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 77,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 78,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 79,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 31, 0, 0 }, { 1, 32, 32 } },
            { { 31, 0, 0 }, { 1, 32, 32 } },
            { { 0, 0, 32 }, { 40, 32, 1 } },
            {},
            { { 0, 0, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 80,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 81,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 82,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 83,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 84,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 85,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 86,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 87,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 88,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 89,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 90,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 91,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 92,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 93,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 94,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 95,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 96,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 97,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 98,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 99,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 100,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 101,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 102,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 103,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 104,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 105,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 106,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 107,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 108,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 109,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 110,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 111,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 0, 31, 0 }, { 32, 1, 32 } },
            { { 0, 31, 0 }, { 32, 1, 32 } },
        }},
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 31, 0, 0 }, { 1, 32, 32 } },
            { { 31, 0, 0 }, { 1, 32, 32 } },
        }},
        {{
            { { 16, 0, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 112,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 113,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 114,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 115,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 116,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 117,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 118,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 119,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 120,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 121,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 122,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 123,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 124,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 125,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 126,
            SPR_G2_SINGLE_RAIL_TRACK_GENTLE_LARGE_CURVE_BANKED + 127,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 0, 31, 0 }, { 32, 1, 32 } },
            { { 0, 31, 0 }, { 32, 1, 32 } },
        }},
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 31, 0, 0 }, { 1, 32, 32 } },
            { { 31, 0, 0 }, { 1, 32, 32 } },
        }},
        {{
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BRAKE_DIAGONAL + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BRAKE_DIAGONAL + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BRAKE_DIAGONAL + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_BRAKE_DIAGONAL + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE_DIAGONAL + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE_DIAGONAL + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE_DIAGONAL + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_BLOCK_BRAKE_DIAGONAL + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kDown25BrakesSprites = {{
        SPR_G2_SINGLE_RAIL_GENTLE_BRAKE + 2,
        SPR_G2_SINGLE_RAIL_GENTLE_BRAKE + 3,
        SPR_G2_SINGLE_RAIL_GENTLE_BRAKE + 0,
        SPR_G2_SINGLE_RAIL_GENTLE_BRAKE + 1,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kDown25BrakesBoundBoxes = {{
        { { 0, 6, 3 }, { 32, 20, 2 } },
        { { 6, 0, 3 }, { 20, 32, 2 } },
        { { 0, 6, 3 }, { 32, 20, 2 } },
        { { 6, 0, 3 }, { 20, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kDiagFlatToUp60LongBaseSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 3,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 5,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 7,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 9,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 11,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections>
    kDiagFlatToUp60LongBaseOffsets = {{
        {{
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
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
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
        }},
        {{
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
    kDiagFlatToUp60LongBaseBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
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
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 80 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -20, -20, 8 }, { 24, 24, 1 } },
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kDiagUp60ToFlatLongBaseSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 13,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 15,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 18,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 19,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 20,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 21,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 22,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_FLAT_TO_STEEP_DIAGONAL + 23,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections>
    kDiagUp60ToFlatLongBaseOffsets = {{
        {{
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
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
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
        }},
        {{
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
    kDiagUp60ToFlatLongBaseBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
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
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -8, -8, 0 }, { 16, 16, 3 } },
            {},
            {},
            { { -8, -8, 56 }, { 16, 16, 3 } },
            {},
            {},
            { { -16, -16, 24 }, { 24, 24, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kLeftEighthDiveLoopUpToOrthogonalSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 0,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 1,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 2,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 3,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 5,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 6,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 7,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 8,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 9,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 10,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 11,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 13,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 14,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 15,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 6>, kNumOrthogonalDirections>
    kLeftEighthDiveLoopUpToOrthogonalOffsets = {{
        {{
            {},
            { -16, -16, 0 }, 
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
        {{
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
        {{
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kLeftEighthDiveLoopUpToOrthogonalBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            { { 0, 31, 0 }, { 32, 1, 64 } },
            { { 0, 31, 0 }, { 32, 1, 64 } },
            { { 0, 6, 26 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, 44 }, { 32, 32, 1 } },
            {},
            {},
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 6, 0, 26 }, { 20, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            { { 2, 2, 8 }, { 28, 28, 1 } },
            { { 0, 0, 0 }, { 32, 1, 64 } },
            { { 0, 6, 26 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, -8 }, { 32, 32, 1 } },
            {},
            {},
            { { 2, 2, 12 }, { 28, 28, 1 } },
            { { 6, 0, 58 }, { 20, 32, 1 } },
            { { 6, 0, 26 }, { 20, 32, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kRightEighthDiveLoopUpToOrthogonalSprites = {{
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 16,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 17,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 18,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 19,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 21,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 22,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 23,
        }},
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 24,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 25,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 26,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 27,
        }},
        {{
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 29,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 30,
            SPR_G2_SINGLE_RAIL_TRACK_DIVE_LOOP + 31,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 6>, kNumOrthogonalDirections>
    kRightEighthDiveLoopUpToOrthogonalOffsets = {{
        {{
            {},
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
        {{
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
        {{
            {},
            { -16, -16, 0 }, 
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
        {{
            { -16, -16, 0 }, 
            {},
            {},
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
            { -16, -16, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kRightEighthDiveLoopUpToOrthogonalBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthDiveLoopUpToOrthogonalBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagDown25BrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_GENTLE_DIAG_BRAKE + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_GENTLE_DIAG_BRAKE + 3,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_SINGLE_RAIL_GENTLE_DIAG_BRAKE + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_SINGLE_RAIL_GENTLE_DIAG_BRAKE + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagDown25BrakesOffsets = {{
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
    kDiagDown25BrakesBoundBoxes = {{
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
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false }, // Flat
        { &kBlockBrakesSprites[0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
        { &kBrakesSprites[0], nullptr, &kStationBoundBoxes[0], false }, // BeginStation
        { &kBrakesSprites[0], nullptr, &kStationBoundBoxes[0], false }, // MiddleStation
        { &kUp25Sprites[0][0], nullptr, &kUp25BoundBoxes[0], false }, // Up25
        { &kUp60Sprites[0][0], nullptr, &kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], nullptr, &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0][0], nullptr, &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0][0], nullptr, &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], nullptr, &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0], nullptr, &kLeftQuarterTurn5TilesBoundBoxes[0][0], false }, // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0][0], nullptr, &kFlatToLeftBankBoundBoxes[0][0], false }, // FlatToLeftBank
        { &kFlatToRightBankSprites[0][0], nullptr, &kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { &kBankedLeftQuarterTurn5TilesSprites[0][0][0], nullptr, &kBankedLeftQuarterTurn5TilesBoundBoxes[0][0][0], false }, // BankedLeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // BankedRightQuarterTurn5Tiles
        { &kLeftBankToUp25Sprites[0][0], nullptr, &kLeftBankToUp25BoundBoxes[0][0], false }, // LeftBankToUp25
        { &kRightBankToUp25Sprites[0][0], nullptr, &kRightBankToUp25BoundBoxes[0][0], false }, // RightBankToUp25
        { &kUp25ToLeftBankSprites[0][0], nullptr, &kUp25ToLeftBankBoundBoxes[0][0], false }, // Up25ToLeftBank
        { &kUp25ToRightBankSprites[0][0], nullptr, &kUp25ToRightBankBoundBoxes[0][0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBank
        { &kLeftBankSprites[0], nullptr, &kLeftBankBoundBoxes[0], false }, // LeftBank
        { nullptr, nullptr, nullptr, true }, // RightBank
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0], nullptr, &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0], nullptr, &kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], nullptr, &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], nullptr, &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], nullptr, &kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &kLeftQuarterTurn3TilesUp25Sprites[0][0], &kLeftQuarterTurn3TilesUp25Offsets[0][0], &kLeftQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0], &kRightQuarterTurn3TilesUp25Offsets[0][0], &kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false }, // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true }, // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true }, // RightTwistUpToDown
        { &kHalfLoopUpSprites[0][0], &kHalfLoopUpOffsets[0][0], &kHalfLoopUpBoundBoxes[0][0], false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true }, // HalfLoopDown
        { &kLeftCorkscrewUpSprites[0][0], nullptr, &kLeftCorkscrewUpBoundBoxes[0][0], false }, // LeftCorkscrewUp
        { &kRightCorkscrewUpSprites[0][0], nullptr, &kRightCorkscrewUpBoundBoxes[0][0], false }, // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightCorkscrewDown
        { &kFlatToUp60Sprites[0][0][0], nullptr, &kFlatToUp60BoundBoxes[0][0], false }, // FlatToUp60
        { &kUp60ToFlatSprites[0][0][0], nullptr, &kUp60ToFlatBoundBoxes[0][0], false }, // Up60ToFlat
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
        { &kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownLarge
        { &kLeftQuarterTurn1TileUp60Sprites[0][0], nullptr, &kLeftQuarterTurn1TileUp60BoundBoxes[0][0], false }, // LeftQuarterTurn1TileUp60
        { &kRightQuarterTurn1TileUp60Sprites[0][0], nullptr, &kRightQuarterTurn1TileUp60BoundBoxes[0][0], false }, // RightQuarterTurn1TileUp60
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
        { &kUp25LeftBankedSprites[0], nullptr, &kUp25LeftBankedBoundBoxes[0], false }, // Up25LeftBanked
        { &kUp25RightBankedSprites[0], nullptr, &kUp25RightBankedBoundBoxes[0], false }, // Up25RightBanked
        { nullptr, nullptr, nullptr, false }, // Waterfall
        { nullptr, nullptr, nullptr, false }, // Rapids
        { &kFlatSprites[0][0], nullptr, &kStationBoundBoxes[0], false }, // OnRidePhoto
        { nullptr, nullptr, nullptr, true }, // Down25LeftBanked
        { nullptr, nullptr, nullptr, true }, // Down25RightBanked
        { nullptr, nullptr, nullptr, false }, // Watersplash
        { &kFlatToUp60LongBaseSprites[0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // Whirlpool
        { nullptr, nullptr, nullptr, true }, // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false }, // CableLiftHill
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallVertical
        { &kUp90Sprites[0][0], nullptr, &kUp90BoundBoxes[0][0], false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { &kUp60ToUp90Sprites[0][0], nullptr, &kUp60ToUp90BoundBoxes[0][0], false }, // Up60ToUp90
        { &kDown90ToDown60Sprites[0], nullptr, &kDown90ToDown60BoundBoxes[0], false }, // Down90ToDown60
        { &kUp90ToUp60Sprites[0], nullptr, &kUp90ToUp60BoundBoxes[0], false }, // Up90ToUp60
        { &kDown60ToDown90Sprites[0][0], nullptr, &kDown60ToDown90BoundBoxes[0][0], false }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &kLeftEighthToDiagSprites[0][0], nullptr, &kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0], nullptr, &kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { &kLeftEighthBankToDiagSprites[0][0], nullptr, &kLeftEighthBankToDiagBoundBoxes[0][0], false }, // LeftEighthBankToDiag
        { &kRightEighthBankToDiagSprites[0][0], nullptr, &kRightEighthBankToDiagBoundBoxes[0][0], false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0], &kDiagFlatOffsets[0][0], &kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0], &kDiagUp25Offsets[0][0], &kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { &kDiagUp60Sprites[0][0][0], &kDiagUp60Offsets[0][0], &kDiagUp60BoundBoxes[0][0], false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0][0], &kDiagUp25ToUp60Offsets[0][0], &kDiagUp25ToUp60BoundBoxes[0][0], false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0][0], &kDiagUp60ToUp25Offsets[0][0], &kDiagUp60ToUp25BoundBoxes[0][0], false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0], &kDiagUp25ToFlatOffsets[0][0], &kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown60
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToFlat
        { &kDiagFlatToUp60Sprites[0][0][0], &kDiagFlatToUp60Offsets[0][0], &kDiagFlatToUp60BoundBoxes[0][0], false }, // DiagFlatToUp60
        { &kDiagUp60ToFlatSprites[0][0][0], &kDiagUp60ToFlatOffsets[0][0], &kDiagUp60ToFlatBoundBoxes[0][0], false }, // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlat
        { &kDiagFlatToLeftBankSprites[0][0][0], &kDiagFlatToLeftBankOffsets[0][0][0], &kDiagFlatToLeftBankBoundBoxes[0][0][0], false }, // DiagFlatToLeftBank
        { &kDiagFlatToRightBankSprites[0][0][0], &kDiagFlatToRightBankOffsets[0][0][0], &kDiagFlatToRightBankBoundBoxes[0][0][0], false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToFlat
        { &kDiagLeftBankToUp25Sprites[0][0][0], &kDiagLeftBankToUp25Offsets[0][0][0], &kDiagLeftBankToUp25BoundBoxes[0][0][0], false }, // DiagLeftBankToUp25
        { &kDiagRightBankToUp25Sprites[0][0][0], &kDiagRightBankToUp25Offsets[0][0][0], &kDiagRightBankToUp25BoundBoxes[0][0][0], false }, // DiagRightBankToUp25
        { &kDiagUp25ToLeftBankSprites[0][0][0], &kDiagUp25ToLeftBankOffsets[0][0][0], &kDiagUp25ToLeftBankBoundBoxes[0][0][0], false }, // DiagUp25ToLeftBank
        { &kDiagUp25ToRightBankSprites[0][0][0], &kDiagUp25ToRightBankOffsets[0][0][0], &kDiagUp25ToRightBankBoundBoxes[0][0][0], false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBank
        { &kDiagLeftBankSprites[0][0], &kDiagLeftBankOffsets[0][0], &kDiagLeftBankBoundBoxes[0][0], false }, // DiagLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagRightBank
        { nullptr, nullptr, nullptr, false }, // LogFlumeReverser
        { nullptr, nullptr, nullptr, false }, // SpinningTunnel
        { &kLeftBarrelRollUpToDownSprites[0][0][0], nullptr, &kLeftBarrelRollUpToDownBoundBoxes[0][0][0], false }, // LeftBarrelRollUpToDown
        { &kRightBarrelRollUpToDownSprites[0][0][0], nullptr, &kRightBarrelRollUpToDownBoundBoxes[0][0][0], false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true }, // RightBarrelRollDownToUp
        { &kLeftBankToLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25Offsets[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { &kRightBankToRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25Offsets[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false }, // PoweredLift
        { &kLeftLargeHalfLoopUpSprites[0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftLargeHalfLoopUp
        { &kRightLargeHalfLoopUpSprites[0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightLargeHalfLoopUp
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
        { &kUp90ToInvertedFlatQuarterLoopSprites[0][0], nullptr, &kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &kInvertedFlatToDown90QuarterLoopSprites[0][0], nullptr, &kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0], false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0], nullptr, &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
        { &kLeftBankedQuarterTurn3TileUp25Sprites[0][0], &kLeftBankedQuarterTurn3TileUp25Offsets[0][0], &kLeftBankedQuarterTurn3TileUp25BoundBoxes[0][0], false }, // LeftBankedQuarterTurn3TileUp25
        { &kRightBankedQuarterTurn3TileUp25Sprites[0][0], &kRightBankedQuarterTurn3TileUp25Offsets[0][0], &kRightBankedQuarterTurn3TileUp25BoundBoxes[0][0], false }, // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3TileDown25
        { &kLeftBankedQuarterTurn5TileUp25Sprites[0][0], nullptr, &kLeftBankedQuarterTurn5TileUp25BoundBoxes[0][0], false }, // LeftBankedQuarterTurn5TileUp25
        { &kRightBankedQuarterTurn5TileUp25Sprites[0][0], nullptr, &kRightBankedQuarterTurn5TileUp25BoundBoxes[0][0], false }, // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn5TileDown25
        { &kUp25ToLeftBankedUp25Sprites[0][0], nullptr, &kUp25ToLeftBankedUp25BoundBoxes[0][0], false }, // Up25ToLeftBankedUp25
        { &kUp25ToRightBankedUp25Sprites[0][0], nullptr, &kUp25ToRightBankedUp25BoundBoxes[0][0], false }, // Up25ToRightBankedUp25
        { &kLeftBankedUp25ToUp25Sprites[0][0], nullptr, &kLeftBankedUp25ToUp25BoundBoxes[0][0], false }, // LeftBankedUp25ToUp25
        { &kRightBankedUp25ToUp25Sprites[0][0], nullptr, &kRightBankedUp25ToUp25BoundBoxes[0][0], false }, // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToDown25
        { &kLeftBankedFlatToLeftBankedUp25Sprites[0], nullptr, &kLeftBankedFlatToLeftBankedUp25BoundBoxes[0], false }, // LeftBankedFlatToLeftBankedUp25
        { &kRightBankedFlatToRightBankedUp25Sprites[0], nullptr, &kRightBankedFlatToRightBankedUp25BoundBoxes[0], false }, // RightBankedFlatToRightBankedUp25
        { &kLeftBankedUp25ToLeftBankedFlatSprites[0], nullptr, &kLeftBankedUp25ToLeftBankedFlatBoundBoxes[0], false }, // LeftBankedUp25ToLeftBankedFlat
        { &kRightBankedUp25ToRightBankedFlatSprites[0], nullptr, &kRightBankedUp25ToRightBankedFlatBoundBoxes[0], false }, // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToRightBankedFlat
        { &kFlatToLeftBankedUp25Sprites[0][0], nullptr, &kFlatToLeftBankedUp25BoundBoxes[0][0], false }, // FlatToLeftBankedUp25
        { &kFlatToRightBankedUp25Sprites[0][0], nullptr, &kFlatToRightBankedUp25BoundBoxes[0][0], false }, // FlatToRightBankedUp25
        { &kLeftBankedUp25ToFlatSprites[0][0], nullptr, &kLeftBankedUp25ToFlatBoundBoxes[0][0], false }, // LeftBankedUp25ToFlat
        { &kRightBankedUp25ToFlatSprites[0][0], nullptr, &kRightBankedUp25ToFlatBoundBoxes[0][0], false }, // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToFlat
        { &kLeftQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &kLeftQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // LeftQuarterTurn1TileUp90
        { &kRightQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &kRightQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // RightQuarterTurn1TileUp90
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
        { &kLeftLargeCorkscrewUpSprites[0][0], nullptr, &kLeftLargeCorkscrewUpBoundBoxes[0][0], false }, // LeftLargeCorkscrewUp
        { &kRightLargeCorkscrewUpSprites[0][0], nullptr, &kRightLargeCorkscrewUpBoundBoxes[0][0], false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { &kLeftMediumHalfLoopUpSprites[0][0], nullptr, &kLeftMediumHalfLoopUpBoundBoxes[0][0], false }, // LeftMediumHalfLoopUp
        { &kRightMediumHalfLoopUpSprites[0][0], nullptr, &kRightMediumHalfLoopUpBoundBoxes[0][0], false }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightMediumHalfLoopDown
        { &kLeftZeroGRollUpSprites[0][0][0], nullptr, &kLeftZeroGRollUpBoundBoxes[0][0][0], false }, // LeftZeroGRollUp
        { &kRightZeroGRollUpSprites[0][0][0], nullptr, &kRightZeroGRollUpBoundBoxes[0][0][0], false }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true }, // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true }, // RightZeroGRollDown
        { &kLeftLargeZeroGRollUpSprites[0][0][0], nullptr, &kLeftLargeZeroGRollUpBoundBoxes[0][0][0], false }, // LeftLargeZeroGRollUp
        { &kRightLargeZeroGRollUpSprites[0][0][0], nullptr, &kRightLargeZeroGRollUpBoundBoxes[0][0][0], false }, // RightLargeZeroGRollUp
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
        { &kLeftEighthToDiagUp25Sprites[0][0], nullptr, &kLeftEighthToDiagUp25BoundBoxes[0][0], false }, // LeftEighthToDiagUp25
        { &kRightEighthToDiagUp25Sprites[0][0], nullptr, &kRightEighthToDiagUp25BoundBoxes[0][0], false }, // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToDiagDown25
        { &kLeftEighthToOrthogonalUp25Sprites[0][0], nullptr, &kLeftEighthToOrthogonalUp25BoundBoxes[0][0], false }, // LeftEighthToOrthogonalUp25
        { &kRightEighthToOrthogonalUp25Sprites[0][0], nullptr, &kRightEighthToOrthogonalUp25BoundBoxes[0][0], false }, // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonalDown25
        { &kDiagUp25ToLeftBankedUp25Sprites[0][0][0], &kDiagUp25ToLeftBankedUp25Offsets[0][0][0], &kDiagUp25ToLeftBankedUp25BoundBoxes[0][0][0], false }, // DiagUp25ToLeftBankedUp25
        { &kDiagUp25ToRightBankedUp25Sprites[0][0][0], &kDiagUp25ToRightBankedUp25Offsets[0][0][0], &kDiagUp25ToRightBankedUp25BoundBoxes[0][0][0], false }, // DiagUp25ToRightBankedUp25
        { &kDiagLeftBankedUp25ToUp25Sprites[0][0][0], &kDiagLeftBankedUp25ToUp25Offsets[0][0][0], &kDiagLeftBankedUp25ToUp25BoundBoxes[0][0][0], false }, // DiagLeftBankedUp25ToUp25
        { &kDiagRightBankedUp25ToUp25Sprites[0][0][0], &kDiagRightBankedUp25ToUp25Offsets[0][0][0], &kDiagRightBankedUp25ToUp25BoundBoxes[0][0][0], false }, // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToDown25
        { &kDiagLeftBankedFlatToLeftBankedUp25Sprites[0][0], &kDiagLeftBankedFlatToLeftBankedUp25Offsets[0][0], &kDiagLeftBankedFlatToLeftBankedUp25BoundBoxes[0][0], false }, // DiagLeftBankedFlatToLeftBankedUp25
        { &kDiagRightBankedFlatToRightBankedUp25Sprites[0][0], &kDiagRightBankedFlatToRightBankedUp25Offsets[0][0], &kDiagRightBankedFlatToRightBankedUp25BoundBoxes[0][0], false }, // DiagRightBankedFlatToRightBankedUp25
        { &kDiagLeftBankedUp25ToLeftBankedFlatSprites[0][0], &kDiagLeftBankedUp25ToLeftBankedFlatOffsets[0][0], &kDiagLeftBankedUp25ToLeftBankedFlatBoundBoxes[0][0], false }, // DiagLeftBankedUp25ToLeftBankedFlat
        { &kDiagRightBankedUp25ToRightBankedFlatSprites[0][0], &kDiagRightBankedUp25ToRightBankedFlatOffsets[0][0], &kDiagRightBankedUp25ToRightBankedFlatBoundBoxes[0][0], false }, // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToRightBankedFlat
        { &kDiagFlatToLeftBankedUp25Sprites[0][0][0], &kDiagFlatToLeftBankedUp25Offsets[0][0][0], &kDiagFlatToLeftBankedUp25BoundBoxes[0][0][0], false }, // DiagFlatToLeftBankedUp25
        { &kDiagFlatToRightBankedUp25Sprites[0][0][0], &kDiagFlatToRightBankedUp25Offsets[0][0][0], &kDiagFlatToRightBankedUp25BoundBoxes[0][0][0], false }, // DiagFlatToRightBankedUp25
        { &kDiagLeftBankedUp25ToFlatSprites[0][0][0], &kDiagLeftBankedUp25ToFlatOffsets[0][0][0], &kDiagLeftBankedUp25ToFlatBoundBoxes[0][0][0], false }, // DiagLeftBankedUp25ToFlat
        { &kDiagRightBankedUp25ToFlatSprites[0][0][0], &kDiagRightBankedUp25ToFlatOffsets[0][0][0], &kDiagRightBankedUp25ToFlatBoundBoxes[0][0][0], false }, // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankedDown25ToFlat
        { &kDiagUp25LeftBankedSprites[0][0], &kDiagUp25LeftBankedOffsets[0][0], &kDiagUp25LeftBankedBoundBoxes[0][0], false }, // DiagUp25LeftBanked
        { &kDiagUp25RightBankedSprites[0][0], &kDiagUp25RightBankedOffsets[0][0], &kDiagUp25RightBankedBoundBoxes[0][0], false }, // DiagUp25RightBanked
        { nullptr, nullptr, nullptr, true }, // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr, true }, // DiagDown25RightBanked
        { &kLeftEighthBankToDiagUp25Sprites[0][0], nullptr, &kLeftEighthBankToDiagUp25BoundBoxes[0][0], false }, // LeftEighthBankToDiagUp25
        { &kRightEighthBankToDiagUp25Sprites[0][0], nullptr, &kRightEighthBankToDiagUp25BoundBoxes[0][0], false }, // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToDiagDown25
        { &kLeftEighthBankToOrthogonalUp25Sprites[0][0], nullptr, &kLeftEighthBankToOrthogonalUp25BoundBoxes[0][0], false }, // LeftEighthBankToOrthogonalUp25
        { &kRightEighthBankToOrthogonalUp25Sprites[0][0], nullptr, &kRightEighthBankToOrthogonalUp25BoundBoxes[0][0], false }, // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonalDown25
        { &kDiagBrakesSprites[0][0], &kDiagBrakesOffsets[0][0], &kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0], &kDiagBlockBrakesOffsets[0][0], &kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
        { &kDown25BrakesSprites[0], nullptr, &kDown25BrakesBoundBoxes[0], false }, // Down25Brakes
        { nullptr, nullptr, nullptr, false }, // DiagBooster
        { &kDiagFlatToUp60LongBaseSprites[0][0], &kDiagFlatToUp60LongBaseOffsets[0][0], &kDiagFlatToUp60LongBaseBoundBoxes[0][0], false }, // DiagFlatToUp60LongBase
        { &kDiagUp60ToFlatLongBaseSprites[0][0], &kDiagUp60ToFlatLongBaseOffsets[0][0], &kDiagUp60ToFlatLongBaseBoundBoxes[0][0], false }, // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlatLongBase
        { &kLeftEighthDiveLoopUpToOrthogonalSprites[0][0], &kLeftEighthDiveLoopUpToOrthogonalOffsets[0][0], &kLeftEighthDiveLoopUpToOrthogonalBoundBoxes[0][0], false }, // LeftEighthDiveLoopUpToOrthogonal
        { &kRightEighthDiveLoopUpToOrthogonalSprites[0][0], &kRightEighthDiveLoopUpToOrthogonalOffsets[0][0], &kRightEighthDiveLoopUpToOrthogonalBoundBoxes[0][0], false }, // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, true }, // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, true }, // RightEighthDiveLoopDownToDiag
        { &kDiagDown25BrakesSprites[0][0], &kDiagDown25BrakesOffsets[0][0], &kDiagDown25BrakesBoundBoxes[0][0], false }, // DiagDown25Brakes
    } };
    // clang-format on
} // namespace OpenRCT2::SingleRailRollerCoaster
