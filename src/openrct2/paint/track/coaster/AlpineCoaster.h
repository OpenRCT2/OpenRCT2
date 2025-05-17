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

namespace OpenRCT2::AlpineCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            SPR_G2_ALPINE_TRACK_FLAT + 0,
            SPR_G2_ALPINE_TRACK_FLAT + 1,
            SPR_G2_ALPINE_TRACK_FLAT + 0,
            SPR_G2_ALPINE_TRACK_FLAT + 1,
        }},
        {{
            SPR_G2_ALPINE_LIFT_TRACK_FLAT + 0,
            SPR_G2_ALPINE_LIFT_TRACK_FLAT + 1,
            SPR_G2_ALPINE_LIFT_TRACK_FLAT + 2,
            SPR_G2_ALPINE_LIFT_TRACK_FLAT + 3,
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
            SPR_G2_ALPINE_TRACK_GENTLE + 8,
            SPR_G2_ALPINE_TRACK_GENTLE + 9,
            SPR_G2_ALPINE_TRACK_GENTLE + 10,
            SPR_G2_ALPINE_TRACK_GENTLE + 11,
        }},
        {{
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 8,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 9,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 10,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 11,
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
    kFlatToUp25Sprites = {{
        {{
            SPR_G2_ALPINE_TRACK_GENTLE + 0,
            SPR_G2_ALPINE_TRACK_GENTLE + 1,
            SPR_G2_ALPINE_TRACK_GENTLE + 2,
            SPR_G2_ALPINE_TRACK_GENTLE + 3,
        }},
        {{
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 0,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 1,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 2,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 3,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            SPR_G2_ALPINE_TRACK_GENTLE + 4,
            SPR_G2_ALPINE_TRACK_GENTLE + 5,
            SPR_G2_ALPINE_TRACK_GENTLE + 6,
            SPR_G2_ALPINE_TRACK_GENTLE + 7,
        }},
        {{
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 4,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 5,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 6,
            SPR_G2_ALPINE_LIFT_TRACK_GENTLE + 7,
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
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 0,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 1,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 3,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 4,
        }},
        {{
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 5,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 6,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 7,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 8,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 9,
        }},
        {{
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 11,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 12,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 13,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 14,
        }},
        {{
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 15,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 16,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 17,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 18,
            SPR_G2_ALPINE_TRACK_MEDIUM_CURVE + 19,
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
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 0, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 1, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 2, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 3, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 4, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 5, kImageIndexUndefined, }},
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
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 7, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 8, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 9, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 10, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 11, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 0, SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 2, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 4, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 7, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 9, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 12, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 14, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 15, SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 16, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 18, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 20, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_MEDIUM_CURVE_BANKED + 21, kImageIndexUndefined, }},
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
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 12, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 13, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 14, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 15, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 16, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 17, kImageIndexUndefined, }},
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
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 18, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 19, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 20, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 21, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 22, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 23, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 24, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 25, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 26, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 27, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 28, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 29, kImageIndexUndefined, }},
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
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 30, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 31, kImageIndexUndefined, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 32, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 33, }},
        {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 34, SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 35, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 36,
        SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 37,
        SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 38,
        SPR_G2_ALPINE_TRACK_BANK_TRANSITION + 39,
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
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 0,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 1,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 3,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 4,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 5,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 6,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 7,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 8,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 9,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 11,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 12,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 13,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 14,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 15,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 16,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 17,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 18,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 19,
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
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 20,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 21,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 23,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 24,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 25,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 26,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 27,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 28,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 29,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 31,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 32,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 33,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 34,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 35,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 36,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 38,
            SPR_G2_ALPINE_TRACK_GENTLE_MEDIUM_CURVE + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 0,
            SPR_G2_ALPINE_TRACK_S_BEND + 1,
            SPR_G2_ALPINE_TRACK_S_BEND + 2,
            SPR_G2_ALPINE_TRACK_S_BEND + 3,
        }},
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 4,
            SPR_G2_ALPINE_TRACK_S_BEND + 5,
            SPR_G2_ALPINE_TRACK_S_BEND + 6,
            SPR_G2_ALPINE_TRACK_S_BEND + 7,
        }},
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 3,
            SPR_G2_ALPINE_TRACK_S_BEND + 2,
            SPR_G2_ALPINE_TRACK_S_BEND + 1,
            SPR_G2_ALPINE_TRACK_S_BEND + 0,
        }},
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 7,
            SPR_G2_ALPINE_TRACK_S_BEND + 6,
            SPR_G2_ALPINE_TRACK_S_BEND + 5,
            SPR_G2_ALPINE_TRACK_S_BEND + 4,
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
            SPR_G2_ALPINE_TRACK_S_BEND + 8,
            SPR_G2_ALPINE_TRACK_S_BEND + 9,
            SPR_G2_ALPINE_TRACK_S_BEND + 10,
            SPR_G2_ALPINE_TRACK_S_BEND + 11,
        }},
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 12,
            SPR_G2_ALPINE_TRACK_S_BEND + 13,
            SPR_G2_ALPINE_TRACK_S_BEND + 14,
            SPR_G2_ALPINE_TRACK_S_BEND + 15,
        }},
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 11,
            SPR_G2_ALPINE_TRACK_S_BEND + 10,
            SPR_G2_ALPINE_TRACK_S_BEND + 9,
            SPR_G2_ALPINE_TRACK_S_BEND + 8,
        }},
        {{
            SPR_G2_ALPINE_TRACK_S_BEND + 15,
            SPR_G2_ALPINE_TRACK_S_BEND + 14,
            SPR_G2_ALPINE_TRACK_S_BEND + 13,
            SPR_G2_ALPINE_TRACK_S_BEND + 12,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 0,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 1,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 2,
        }},
        {{
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 3,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 4,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 5,
        }},
        {{
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 7,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 8,
        }},
        {{
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 9,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 10,
            SPR_G2_ALPINE_TRACK_SMALL_CURVE + 11,
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
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 0, SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 2, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 8, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 9, SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 10, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 12, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_CURVE_BANKED + 13, kImageIndexUndefined, }},
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
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 1,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 3,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 5,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 7,
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
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 8,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 9,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 11,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 13,
        }},
        {{
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_GENTLE_SMALL_CURVE + 15,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 0, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 3, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 13, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 5, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 6, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 0, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 8, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 9, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 10, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 5, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 13, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 8, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 9, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 10, }},
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
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 15, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 16, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 19, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 20, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 19, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 20, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 21, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 22, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 23, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 21, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 22, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 23, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 24, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 25, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 26, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 27, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 24, SPR_G2_ALPINE_TRACK_SMALL_HELIX + 25, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 26, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 27, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 15, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_SMALL_HELIX + 16, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 0, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 4, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 5, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 20, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 21, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 7, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 9, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 10, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 0, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 2, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 4, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 14, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 15, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 16, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 7, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 9, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 18, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 19, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 20, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 21, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 12, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 14, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 15, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 16, }},
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
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 22, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 23, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 24, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 25, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 26, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 27, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 28, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 29, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 30, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 31, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 32, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 27, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 28, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 29, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 30, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 31, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 32, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 33, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 34, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 35, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 36, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 37, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 33, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 34, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 35, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 36, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 37, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 38, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 39, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 40, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 41, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 42, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 43, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 38, SPR_G2_ALPINE_TRACK_LARGE_HELIX + 39, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 40, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 41, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 42, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 43, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 22, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 23, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 24, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 25, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_LARGE_HELIX + 26, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        SPR_G2_ALPINE_TRACK_FLAT + 0,
        SPR_G2_ALPINE_TRACK_FLAT + 1,
        SPR_G2_ALPINE_TRACK_FLAT + 0,
        SPR_G2_ALPINE_TRACK_FLAT + 1,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 0,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 1,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 4,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 5,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 8,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 9,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 12,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 13,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 15,
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
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 16,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 17,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 20,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 21,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 24,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 25,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 28,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 29,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 0,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 1,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 2,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 3,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 4,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 5,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 6,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 7,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 8,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 9,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 10,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 11,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 12,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 13,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 14,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 15,
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
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 16,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 17,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 18,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 19,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 20,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 21,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 23,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 24,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 25,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 27,
        }},
        {{
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 28,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 29,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_LARGE_CURVE_BANKED + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_TRACK_FLAT_DIAGONAL + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_LIFT_TRACK_FLAT_DIAGONAL + 3,
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
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 8,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 9,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 10,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 11,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 8,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 9,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 10,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 11,
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
    kDiagFlatToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 0,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 1,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 3,
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
    kDiagUp25ToFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 6,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_TRACK_GENTLE_DIAGONAL + 7,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 6,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_ALPINE_LIFT_TRACK_GENTLE_DIAGONAL + 7,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 0, SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 4, kImageIndexUndefined, }},
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
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 7, SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 9, kImageIndexUndefined, }},
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
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 10, SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 14, kImageIndexUndefined, }},
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
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 17, SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 19, kImageIndexUndefined, }},
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
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 20, SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 21, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 22, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 23, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 24, kImageIndexUndefined, }},
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
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 25, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 26, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 27, SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 28, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 29, kImageIndexUndefined, }},
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
            SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 30,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 31,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 32,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_ALPINE_TRACK_BANK_TRANSITION_DIAGONAL + 33,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 1, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 2, SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 8, kImageIndexUndefined, }},
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
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 13, SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 14, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 16, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_ALPINE_TRACK_TURN_BANK_TRANSITION + 17, kImageIndexUndefined, }},
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false }, // Flat
        { &kFlatSprites[0][0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
        { &kFlatSprites[0][0], nullptr, &kStationBoundBoxes[0], false }, // BeginStation
        { &kFlatSprites[0][0], nullptr, &kStationBoundBoxes[0], false }, // MiddleStation
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
        { &kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpLarge
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
        { nullptr, nullptr, nullptr, false }, // Down90ToDown60
        { nullptr, nullptr, nullptr, false }, // Up90ToUp60
        { nullptr, nullptr, nullptr, false }, // Down60ToDown90
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
        { nullptr, nullptr, nullptr, false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToUp60
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToUp25
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
        { nullptr, nullptr, nullptr, false }, // LeftBarrelRollUpToDown
        { nullptr, nullptr, nullptr, false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true }, // RightBarrelRollDownToUp
        { &kLeftBankToLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25Offsets[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { &kRightBankToRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25Offsets[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightBankToRightQuarterTurn3TilesUp25
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
} // namespace OpenRCT2::AlpineCoaster
