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

namespace OpenRCT2::StandUpRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            25229,
            25230,
            25229,
            25230,
        }},
        {{
            25383,
            25384,
            25453,
            25454,
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
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kStationSprites = {{
        25567,
        25568,
        25567,
        25568,
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
            25231,
            25243,
            25235,
            25245,
        }},
        {{
            25385,
            25397,
            25389,
            25399,
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
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp60Sprites = {{
        {{
            25232,
            25244,
            25236,
            25246,
        }},
        {{
            25386,
            25398,
            25390,
            25400,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp60Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
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
            25233,
            25247,
            25237,
            25249,
        }},
        {{
            25387,
            25401,
            25391,
            25403,
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
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToUp60Sprites = {{
        {{
            {{ 25234, kImageIndexUndefined, }},
            {{ 25277, 25248, }},
            {{ 25275, 25238, }},
            {{ 25250, kImageIndexUndefined, }},
        }},
        {{
            {{ 25388, kImageIndexUndefined, }},
            {{ 25411, 25402, }},
            {{ 25409, 25392, }},
            {{ 25404, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Offsets = {{
        {{ { 0, 6, 0 }, {} }},
        {{ { 6, 0, 0 }, { 6, 0, 0 } }},
        {{ { 0, 6, 0 }, { 0, 6, 0 } }},
        {{ { 6, 0, 0 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 27, 0, 0 }, { 1, 32, 66 } }, { { 6, 0, 0 }, { 20, 32, 3 } } }},
        {{ { { 0, 27, 0 }, { 32, 1, 66 } }, { { 0, 6, 0 }, { 32, 20, 3 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToUp25Sprites = {{
        {{
            {{ 25242, kImageIndexUndefined, }},
            {{ 25278, 25254, }},
            {{ 25276, 25240, }},
            {{ 25252, kImageIndexUndefined, }},
        }},
        {{
            {{ 25396, kImageIndexUndefined, }},
            {{ 25412, 25408, }},
            {{ 25410, 25394, }},
            {{ 25406, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Offsets = {{
        {{ { 0, 6, 0 }, {} }},
        {{ { 6, 0, 0 }, { 6, 0, 0 } }},
        {{ { 0, 6, 0 }, { 0, 6, 0 } }},
        {{ { 6, 0, 0 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 27, 0, 0 }, { 1, 32, 66 } }, { { 6, 0, 0 }, { 20, 32, 3 } } }},
        {{ { { 0, 27, 0 }, { 32, 1, 66 } }, { { 0, 6, 0 }, { 32, 20, 3 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            25241,
            25253,
            25239,
            25251,
        }},
        {{
            25395,
            25407,
            25393,
            25405,
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
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            25264,
            kImageIndexUndefined,
            25263,
            25262,
            kImageIndexUndefined,
            25261,
            25260,
        }},
        {{
            25269,
            kImageIndexUndefined,
            25268,
            25267,
            kImageIndexUndefined,
            25266,
            25265,
        }},
        {{
            25274,
            kImageIndexUndefined,
            25273,
            25272,
            kImageIndexUndefined,
            25271,
            25270,
        }},
        {{
            25259,
            kImageIndexUndefined,
            25258,
            25257,
            kImageIndexUndefined,
            25256,
            25255,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        25281,
        25286,
        25280,
        25283,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        25279,
        25284,
        25282,
        25285,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToRightBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_25464 + 0, 25475, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25463 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25462 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25461 + 0, kImageIndexUndefined, }},
            {{ 25460, kImageIndexUndefined, }},
        }},
        {{
            {{ 25469, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25468 + 0, kImageIndexUndefined, }},
            {{ 25467, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25466 + 0, kImageIndexUndefined, }},
            {{ 25465, kImageIndexUndefined, }},
        }},
        {{
            {{ 25474, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25473 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25472 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25471 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25470 + 0, 25476, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_25459 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25458, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25457 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25456, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25455 + 0, kImageIndexUndefined, }},
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        25315,
        25316,
        25317,
        25318,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        25319,
        25320,
        25321,
        25322,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankToUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        25311,
        25312,
        25313,
        25314,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        25307,
        25308,
        25309,
        25310,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        25323,
        25324,
        25325,
        25326,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>, 2>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            {{
                25347,
                kImageIndexUndefined,
                25348,
                25349,
                kImageIndexUndefined,
                25350,
                25351,
            }},
            {{
                25352,
                kImageIndexUndefined,
                25353,
                25354,
                kImageIndexUndefined,
                25355,
                25356,
            }},
            {{
                25357,
                kImageIndexUndefined,
                25358,
                25359,
                kImageIndexUndefined,
                25360,
                25361,
            }},
            {{
                25362,
                kImageIndexUndefined,
                25363,
                25364,
                kImageIndexUndefined,
                25365,
                25366,
            }},
        }},
        {{
            {{
                25433,
                kImageIndexUndefined,
                25434,
                25435,
                kImageIndexUndefined,
                25436,
                25437,
            }},
            {{
                25438,
                kImageIndexUndefined,
                25439,
                25440,
                kImageIndexUndefined,
                25441,
                25442,
            }},
            {{
                25443,
                kImageIndexUndefined,
                25444,
                25445,
                kImageIndexUndefined,
                25446,
                25447,
            }},
            {{
                25448,
                kImageIndexUndefined,
                25449,
                25450,
                kImageIndexUndefined,
                25451,
                25452,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>, 2>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            {{
                25327,
                kImageIndexUndefined,
                25328,
                25329,
                kImageIndexUndefined,
                25330,
                25331,
            }},
            {{
                25332,
                kImageIndexUndefined,
                25333,
                25334,
                kImageIndexUndefined,
                25335,
                25336,
            }},
            {{
                25337,
                kImageIndexUndefined,
                25338,
                25339,
                kImageIndexUndefined,
                25340,
                25341,
            }},
            {{
                25342,
                kImageIndexUndefined,
                25343,
                25344,
                kImageIndexUndefined,
                25345,
                25346,
            }},
        }},
        {{
            {{
                25413,
                kImageIndexUndefined,
                25414,
                25415,
                kImageIndexUndefined,
                25416,
                25417,
            }},
            {{
                25418,
                kImageIndexUndefined,
                25419,
                25420,
                kImageIndexUndefined,
                25421,
                25422,
            }},
            {{
                25423,
                kImageIndexUndefined,
                25424,
                25425,
                kImageIndexUndefined,
                25426,
                25427,
            }},
            {{
                25428,
                kImageIndexUndefined,
                25429,
                25430,
                kImageIndexUndefined,
                25431,
                25432,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            25371,
            25372,
            25373,
            25374,
        }},
        {{
            25378,
            25377,
            25376,
            25375,
        }},
        {{
            25374,
            25373,
            25372,
            25371,
        }},
        {{
            25375,
            25376,
            25377,
            25378,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
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
            25367,
            25368,
            25369,
            25370,
        }},
        {{
            25382,
            25381,
            25380,
            25379,
        }},
        {{
            25370,
            25369,
            25368,
            25367,
        }},
        {{
            25379,
            25380,
            25381,
            25382,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            25477,
            25478,
            25479,
            25480,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25481,
            25482,
            25483,
            25484,
        }},
        {{
            25485,
            25486,
            25487,
            25488,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25489,
            25490,
            25491,
            25492,
        }},
        {{
            25484,
            25483,
            25482,
            25481,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25480,
            25479,
            25478,
            25477,
        }},
        {{
            25492,
            25491,
            25490,
            25489,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25488,
            25487,
            25486,
            25485,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 16, 0, 0 }, 
            { 0, 0, 32 }, 
            {},
            {},
            { 0, 16, 32 }, 
            { 10, 16, 0 }, 
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 14, 0, 0 }, 
            { 0, 12, 0 }, 
            { 0, 0, 32 }, 
            {},
            {},
            { 16, 0, 32 }, 
            { 16, 16, 0 }, 
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 10, 16, 0 }, 
            { 0, 16, 32 }, 
            {},
            {},
            { 0, 0, 32 }, 
            { 16, 0, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 16, 16, 0 }, 
            { 16, 0, 32 }, 
            {},
            {},
            { 0, 0, 32 }, 
            { 0, 12, 0 }, 
            { 14, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 0 }, { 32, 26, 3 } },
            { { 16, 0, 0 }, { 5, 16, 119 } },
            { { 0, 0, 32 }, { 32, 16, 3 } },
            {},
            {},
            { { 0, 16, 32 }, { 32, 16, 3 } },
            { { 10, 16, 0 }, { 4, 16, 119 } },
            { { 0, 6, 0 }, { 32, 26, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 27, 0, 0 }, { 2, 32, 63 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 0, 0, 32 }, { 16, 32, 3 } },
            {},
            {},
            { { 16, 0, 32 }, { 16, 32, 3 } },
            { { 16, 16, 0 }, { 16, 4, 119 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 7 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 26, 3 } },
            { { 10, 16, 0 }, { 4, 16, 119 } },
            { { 0, 16, 32 }, { 32, 16, 3 } },
            {},
            {},
            { { 0, 0, 32 }, { 32, 16, 3 } },
            { { 16, 0, 0 }, { 5, 16, 119 } },
            { { 0, 0, 0 }, { 32, 26, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 7 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 16, 16, 0 }, { 16, 4, 119 } },
            { { 16, 0, 32 }, { 16, 32, 3 } },
            {},
            {},
            { { 0, 0, 32 }, { 16, 32, 3 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 27, 0, 0 }, { 2, 32, 63 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopSprites = {{
        {{
            25508,
            25507,
            25506,
            25505,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25504,
            25503,
            25502,
            25501,
        }},
        {{
            25500,
            25499,
            25498,
            25497,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25496,
            25495,
            25494,
            25493,
        }},
        {{
            25501,
            25502,
            25503,
            25504,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25505,
            25506,
            25507,
            25508,
        }},
        {{
            25493,
            25494,
            25495,
            25496,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25497,
            25498,
            25499,
            25500,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 16, 16, 0 }, 
            { 0, 16, 32 }, 
            {},
            {},
            { 0, 0, 32 }, 
            { 12, 0, 0 }, 
            { 0, 14, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 16, 10, 0 }, 
            { 16, 0, 32 }, 
            {},
            {},
            { 0, 0, 32 }, 
            { 0, 16, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 14, 0 }, 
            { 12, 0, 0 }, 
            { 0, 0, 32 }, 
            {},
            {},
            { 0, 16, 32 }, 
            { 16, 16, 0 }, 
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 0, 16, 0 }, 
            { 0, 0, 32 }, 
            {},
            {},
            { 16, 0, 32 }, 
            { 16, 10, 0 }, 
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftVerticalLoopBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            25514,
            kImageIndexUndefined,
            25513,
            25512,
        }},
        {{
            25517,
            kImageIndexUndefined,
            25516,
            25515,
        }},
        {{
            25520,
            kImageIndexUndefined,
            25519,
            25518,
        }},
        {{
            25511,
            kImageIndexUndefined,
            25510,
            25509,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesOffsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 16, 0 }, 
            { 0, 6, 0 }, 
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
            {{ SPR_G2_STANDUP_TRACK_25526 + 0, 25533, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25525, kImageIndexUndefined, }},
            {{ 25524, kImageIndexUndefined, }},
        }},
        {{
            {{ 25529, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25528 + 0, kImageIndexUndefined, }},
            {{ 25527, kImageIndexUndefined, }},
        }},
        {{
            {{ 25532, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25531, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25530 + 0, 25534, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_25523 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25522, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25521 + 0, kImageIndexUndefined, }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{
                25546,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25545,
            }},
            {{
                25548,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25547,
            }},
            {{
                25550,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25549,
            }},
            {{
                25544,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25543,
            }},
        }},
        {{
            {{
                25562,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25561,
            }},
            {{
                25564,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25563,
            }},
            {{
                25566,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25565,
            }},
            {{
                25560,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25559,
            }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{
                25535,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25536,
            }},
            {{
                25537,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25538,
            }},
            {{
                25539,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25540,
            }},
            {{
                25541,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25542,
            }},
        }},
        {{
            {{
                25551,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25552,
            }},
            {{
                25553,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25554,
            }},
            {{
                25555,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25556,
            }},
            {{
                25557,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25558,
            }},
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
            25575,
            25576,
            25577,
            25578,
        }},
        {{
            25583,
            25584,
            25585,
            25586,
        }},
        {{
            25582,
            25581,
            25580,
            25579,
        }},
        {{
            25590,
            25589,
            25588,
            25587,
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
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 68 }, { 20, 32, 3 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 0, 0, 32 }, { 16, 32, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 9 } },
            { { 0, 6, 68 }, { 32, 20, 3 } },
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
            25591,
            25592,
            25593,
        }},
        {{
            25594,
            25595,
            25596,
        }},
        {{
            25597,
            25598,
            25599,
        }},
        {{
            25600,
            25601,
            25602,
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
            25603,
            25604,
            25605,
        }},
        {{
            25606,
            25607,
            25608,
        }},
        {{
            25609,
            25610,
            25611,
        }},
        {{
            25612,
            25613,
            25614,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp60Sprites = {{
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 2, SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 1, }},
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 4, SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 3, }},
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 4 } }, {} }},
        {{ { { 29, 0, 0 }, { 1, 32, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 0, 29, 0 }, { 32, 1, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 4 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToFlatSprites = {{
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 6, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 7, SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 8, }},
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 10, SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 9, }},
        {{ SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 11, kImageIndexUndefined, }},
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
            {{ SPR_G2_STANDUP_TRACK_25844 + 0, 25851, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25843 + 0, kImageIndexUndefined, }},
            {{ 25842, kImageIndexUndefined, }},
            {{ 25841, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25840, kImageIndexUndefined, }},
            {{ 25839, kImageIndexUndefined, }},
        }},
        {{
            {{ 25847, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25846, kImageIndexUndefined, }},
            {{ 25845, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25844 + 0, 25851, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25843 + 0, kImageIndexUndefined, }},
            {{ 25842, kImageIndexUndefined, }},
        }},
        {{
            {{ 25850, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25849, kImageIndexUndefined, }},
            {{ 25848, 25852, }},
            {{ 25847, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25846, kImageIndexUndefined, }},
            {{ 25845, kImageIndexUndefined, }},
        }},
        {{
            {{ 25841, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25840, kImageIndexUndefined, }},
            {{ 25839, kImageIndexUndefined, }},
            {{ 25850, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25849, kImageIndexUndefined, }},
            {{ 25848, 25852, }},
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
            {{ SPR_G2_STANDUP_TRACK_25825 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25826, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25827 + 0, kImageIndexUndefined, }},
            {{ 25828, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25829, kImageIndexUndefined, }},
            {{ 25830, 25837, }},
        }},
        {{
            {{ 25828, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25829, kImageIndexUndefined, }},
            {{ 25830, 25837, }},
            {{ 25831, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25832, kImageIndexUndefined, }},
            {{ 25833, kImageIndexUndefined, }},
        }},
        {{
            {{ 25831, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25832, kImageIndexUndefined, }},
            {{ 25833, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25834 + 0, 25838, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25835 + 0, kImageIndexUndefined, }},
            {{ 25836, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_25834 + 0, 25838, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25835 + 0, kImageIndexUndefined, }},
            {{ 25836, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25825 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25826, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25827 + 0, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_25812 + 0, 25823, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25811 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25810 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25809 + 0, kImageIndexUndefined, }},
            {{ 25808, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25807 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25806, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25805 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25804, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25803 + 0, kImageIndexUndefined, }},
        }},
        {{
            {{ 25817, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25816 + 0, kImageIndexUndefined, }},
            {{ 25815, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25814 + 0, kImageIndexUndefined, }},
            {{ 25813, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25812 + 0, 25823, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25811 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25810 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25809 + 0, kImageIndexUndefined, }},
            {{ 25808, kImageIndexUndefined, }},
        }},
        {{
            {{ 25822, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25821 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25820 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25819, kImageIndexUndefined, }},
            {{ 25818, 25824, }},
            {{ 25817, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25816 + 0, kImageIndexUndefined, }},
            {{ 25815, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25814 + 0, kImageIndexUndefined, }},
            {{ 25813, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_25807 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25806, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25805 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25804, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25803 + 0, kImageIndexUndefined, }},
            {{ 25822, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25821 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25820 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25819, kImageIndexUndefined, }},
            {{ 25818, 25824, }},
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
            {{ { { 16, 16, 29 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 29 }, { 32, 16, 1 } }, {} }},
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
            {{ SPR_G2_STANDUP_TRACK_25781 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25782, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25783 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25784, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25785 + 0, kImageIndexUndefined, }},
            {{ 25786, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25787 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25788 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25789, kImageIndexUndefined, }},
            {{ 25790, 25801, }},
        }},
        {{
            {{ 25786, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25787 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25788 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25789, kImageIndexUndefined, }},
            {{ 25790, 25801, }},
            {{ 25791, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25792 + 0, kImageIndexUndefined, }},
            {{ 25793, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25794 + 0, kImageIndexUndefined, }},
            {{ 25795, kImageIndexUndefined, }},
        }},
        {{
            {{ 25791, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25792 + 0, kImageIndexUndefined, }},
            {{ 25793, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25794 + 0, kImageIndexUndefined, }},
            {{ 25795, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25796 + 0, 25802, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25797 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25798 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25799 + 0, kImageIndexUndefined, }},
            {{ 25800, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_25796 + 0, 25802, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25797 + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25798 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25799 + 0, kImageIndexUndefined, }},
            {{ 25800, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25781 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25782, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25783 + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25784, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_25785 + 0, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 0, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 1, }},
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 2, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 3, }},
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 4, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 5, }},
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 6, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 7, }},
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
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 8, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 9, }},
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 10, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 11, }},
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 12, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 13, }},
        {{ SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 14, SPR_G2_STANDUP_TRACK_STEEP_SMALL_CURVE + 15, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        25569,
        25570,
        25569,
        25570,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBrakesOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
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
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 36,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 37,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 38,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 39,
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
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 40,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 41,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 42,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 43,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 6, SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 7, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 9, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 10, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 11, SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 12, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 13, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 14, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 17, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 8 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 48 } } }},
            {{ { { 27, 0, 0 }, { 1, 32, 98 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 48 } } }},
            {{ { { 0, 27, 0 }, { 32, 1, 98 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 8 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 18, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 19, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 20, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 21, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 22, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 23, SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 24, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 26, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 27, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 28, SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 29, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 30, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 31, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 33, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 34, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_FLAT_TO_STEEP + 35, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 98 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 72 }, { 20, 32, 3 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 0, 27, 0 }, { 32, 1, 98 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 72 }, { 32, 20, 3 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp90Sprites = {{
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 9,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 10,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 11,
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
            SPR_G2_STANDUP_TRACK_VERTICAL + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 1,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 2,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 3,
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
            { { 6, 24, 0 }, { 20, 2, 64 } },
            {},
        }},
        {{
            { { 24, 6, 0 }, { 2, 20, 64 } },
            {},
        }},
        {{
            { { 6, 4, 8 }, { 20, 13, 55 } },
            {},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kDown90ToDown60Sprites = {{
        SPR_G2_STANDUP_TRACK_VERTICAL + 2,
        SPR_G2_STANDUP_TRACK_VERTICAL + 3,
        SPR_G2_STANDUP_TRACK_VERTICAL + 0,
        SPR_G2_STANDUP_TRACK_VERTICAL + 1,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kDown90ToDown60BoundBoxes = {{
        { { 24, 6, 0 }, { 2, 20, 64 } },
        { { 6, 4, 8 }, { 20, 13, 55 } },
        { { 4, 6, 8 }, { 13, 20, 55 } },
        { { 6, 24, 0 }, { 20, 2, 64 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp90ToUp60Sprites = {{
        SPR_G2_STANDUP_TRACK_VERTICAL + 4,
        SPR_G2_STANDUP_TRACK_VERTICAL + 5,
        SPR_G2_STANDUP_TRACK_VERTICAL + 6,
        SPR_G2_STANDUP_TRACK_VERTICAL + 7,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp90ToUp60BoundBoxes = {{
        { { 4, 6, 8 }, { 2, 20, 48 } },
        { { 10, 29, 0 }, { 10, 1, 75 } },
        { { 29, 10, 0 }, { 1, 10, 75 } },
        { { 6, 4, 8 }, { 20, 2, 48 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Sprites = {{
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 6,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 7,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 4,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_VERTICAL + 5,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90BoundBoxes = {{
        {{
            { { 29, 10, 0 }, { 1, 10, 75 } },
            {},
        }},
        {{
            { { 6, 0, 8 }, { 20, 32, 3 } },
            {},
        }},
        {{
            { { 0, 6, 8 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 10, 29, 0 }, { 10, 1, 75 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            25643,
            25644,
            25645,
            kImageIndexUndefined,
            25646,
        }},
        {{
            25647,
            25648,
            25649,
            kImageIndexUndefined,
            25650,
        }},
        {{
            25651,
            25652,
            25653,
            kImageIndexUndefined,
            25654,
        }},
        {{
            25655,
            25656,
            25657,
            kImageIndexUndefined,
            25658,
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
            25627,
            25628,
            25629,
            kImageIndexUndefined,
            25630,
        }},
        {{
            25631,
            25632,
            25633,
            kImageIndexUndefined,
            25634,
        }},
        {{
            25635,
            25636,
            25637,
            kImageIndexUndefined,
            25638,
        }},
        {{
            25639,
            25640,
            25641,
            kImageIndexUndefined,
            25642,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            25675,
            SPR_G2_STANDUP_TRACK_25676 + 0,
            SPR_G2_STANDUP_TRACK_25677 + 0,
            kImageIndexUndefined,
            25678,
        }},
        {{
            25679,
            SPR_G2_STANDUP_TRACK_25680 + 0,
            25681,
            kImageIndexUndefined,
            25682,
        }},
        {{
            25683,
            SPR_G2_STANDUP_TRACK_25684 + 0,
            SPR_G2_STANDUP_TRACK_25685 + 0,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_25686 + 0,
        }},
        {{
            SPR_G2_STANDUP_TRACK_25687 + 0,
            25688,
            SPR_G2_STANDUP_TRACK_25689 + 0,
            kImageIndexUndefined,
            25690,
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
            SPR_G2_STANDUP_TRACK_25659 + 0,
            25660,
            SPR_G2_STANDUP_TRACK_25661 + 0,
            kImageIndexUndefined,
            25662,
        }},
        {{
            25663,
            SPR_G2_STANDUP_TRACK_25664 + 0,
            SPR_G2_STANDUP_TRACK_25665 + 0,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_25666 + 0,
        }},
        {{
            25667,
            SPR_G2_STANDUP_TRACK_25668 + 0,
            25669,
            kImageIndexUndefined,
            25670,
        }},
        {{
            25671,
            SPR_G2_STANDUP_TRACK_25672 + 0,
            SPR_G2_STANDUP_TRACK_25673 + 0,
            kImageIndexUndefined,
            25674,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                25691,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25692,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25693,
                kImageIndexUndefined,
            }},
            {{
                25694,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25753,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25754,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25755,
                kImageIndexUndefined,
            }},
            {{
                25756,
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
                25703,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25704,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25705,
                kImageIndexUndefined,
            }},
            {{
                25706,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25765,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25766,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25767,
                kImageIndexUndefined,
            }},
            {{
                25768,
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
                25715,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25716,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25717,
                kImageIndexUndefined,
            }},
            {{
                25718,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25777,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25778,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25779,
                kImageIndexUndefined,
            }},
            {{
                25780,
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
                25695,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25696,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25697,
                kImageIndexUndefined,
            }},
            {{
                25698,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25757,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25758,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25759,
                kImageIndexUndefined,
            }},
            {{
                25760,
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
                25707,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25708,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25709,
                kImageIndexUndefined,
            }},
            {{
                25710,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25769,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25770,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25771,
                kImageIndexUndefined,
            }},
            {{
                25772,
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
                25711,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25712,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25713,
                kImageIndexUndefined,
            }},
            {{
                25714,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25773,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25774,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25775,
                kImageIndexUndefined,
            }},
            {{
                25776,
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
                25699,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25700,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25701,
                kImageIndexUndefined,
            }},
            {{
                25702,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                25761,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                25762,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                25763,
                kImageIndexUndefined,
            }},
            {{
                25764,
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
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp60Sprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 13,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 14,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 15,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60ToFlatSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 18,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_SMALL_FLAT_TO_STEEP + 19,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
            {{ 25723, 25727, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25724, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25725, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 25726, kImageIndexUndefined, }},
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
            {{ 25728, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25729, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25730, 25732, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 25731, kImageIndexUndefined, }},
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
            {{ 25743, 25747, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25744, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25745, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 25746, kImageIndexUndefined, }},
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
            {{ 25748, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25749, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25750, 25752, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 25751, kImageIndexUndefined, }},
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
            {{ 25733, 25737, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25734, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25735, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 25736, kImageIndexUndefined, }},
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
            {{ 25738, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25739, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 25740, 25742, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 25741, kImageIndexUndefined, }},
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
            25719,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25720,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            25721,
            kImageIndexUndefined,
        }},
        {{
            25722,
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
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 0, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 1, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 2, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 4, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 5, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 9, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 10, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 11, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 12, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 14, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 15, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 16, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 17, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftBarrelRollUpToDownBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 26 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 40 }, { 32, 20, 0 } }, {} }},
            {{ { { 0, 6, 40 }, { 32, 20, 0 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 0 } }, { { 6, 0, 28 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 40 }, { 20, 32, 0 } }, {} }},
            {{ { { 6, 0, 40 }, { 20, 32, 0 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 40 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 40 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 40 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 40 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 18, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 19, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 20, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 21, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 22, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 23, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 24, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 25, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 26, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 27, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 28, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 29, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 31, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 32, SPR_G2_STANDUP_TRACK_BARREL_ROLL + 33, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 34, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_BARREL_ROLL + 35, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBarrelRollUpToDownBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 2, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 1, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 3, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 4, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 5, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 6, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 7, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 8, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 12, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 13, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Offsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, { 0, 0, 0 } }},
        }},
        {{
            {{ { 6, 0, 0 }, { 6, 0, 0 } }},
            {{ { 22, 22, 0 }, { 22, 22, 0 } }},
            {{ {}, {} }},
            {{ { 0, 6, 0 }, { 0, 6, 0 } }},
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
            {{ { 16, 16, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 22, 22, 0 }, { 20, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ { { 22, 22, 0 }, { 20, 20, 3 } }, { { 22, 22, 32 }, { 20, 20, 3 } } }},
            {{ {}, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 0 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 15, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 18, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 19, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 20, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 21, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 22, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 23, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 24, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 25, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 27, SPR_G2_STANDUP_TRACK_TURN_BANK_TRANSITION + 26, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Offsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ { 16, 16, 0 }, {} }},
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
            {{ { 22, 22, 0 }, { 22, 22, 0 } }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, { 6, 0, 0 } }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 6, 0 }, { 0, 0, 0 } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 2, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 3, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 5, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 6, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 7, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 9, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 10, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 11, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 12, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 13, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 14, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 17, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 18, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 19, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 20, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 21, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 22, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 23, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 24, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 26, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 27, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 28, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 29, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 30, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 31, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 32, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 33, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 34, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 3 } }, { { -16, 0, 32 }, { 32, 16, 3 } } }},
            {{ { { 0, 0, 2 }, { 32, 16, 3 } }, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, { { 0, 0, 210 }, { 48, 32, 3 } } }},
            {{ { { 0, 16, 32 }, { 32, 16, 0 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 60 }, { 20, 32, 3 } } }},
            {{ { { 0, 0, 0 }, { 16, 32, 0 } }, { { 0, 0, 70 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 200 }, { 16, 32, 0 } }, {} }},
            {{ { { 16, 0, 110 }, { 16, 16, 0 } }, {} }},
            {{ { { 16, 0, 200 }, { 16, 32, 0 } }, {} }},
            {{ { { 16, 0, 32 }, { 16, 32, 0 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 60 }, { 32, 20, 3 } } }},
            {{ { { 0, 16, 0 }, { 32, 16, 0 } }, { { 0, 16, 70 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 200 }, { 32, 16, 0 } }, {} }},
            {{ { { 0, 0, 100 }, { 16, 16, 0 } }, {} }},
            {{ { { 0, 0, 200 }, { 32, 16, 0 } }, {} }},
            {{ { { 0, 0, 32 }, { 32, 16, 0 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 3 } }, { { 0, 0, 210 }, { 16, 48, 3 } } }},
            {{ { { 0, 0, 32 }, { 16, 32, 0 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 35, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 36, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 37, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 38, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 39, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 40, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 41, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 42, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 43, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 44, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 45, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 46, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 47, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 48, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 49, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 50, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 51, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 52, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 53, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 54, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 55, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 56, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 57, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 58, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 59, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 60, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 61, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 62, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 63, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 64, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 65, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 66, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 67, SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 68, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_HALF_LOOP + 69, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 1, SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 2, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 5, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 10, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 11, SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 12, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 13, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopBoundBoxes = {{
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 48 } }, {} }},
            {{ { { -8, 4, 0 }, { 28, 2, 31 } }, { { 0, 0, 48 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 24 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 2, 63 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
        }},
        {{
            {{ { { 24, 6, 8 }, { 2, 20, 31 } }, {} }},
            {{ { { 24, 6, 8 }, { 2, 20, 63 } }, {} }},
            {{ { { 24, 6, 8 }, { 2, 20, 31 } }, {} }},
        }},
        {{
            {{ { { 4, 6, 8 }, { 2, 20, 48 } }, {} }},
            {{ { { 4, -8, 0 }, { 2, 28, 31 } }, { { 0, 0, 48 }, { 20, 32, 1 } } }},
            {{ { { 6, 0, 24 }, { 20, 32, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kInvertedFlatToDown90QuarterLoopSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 3, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 1, SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 2, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 6, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 5, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 4, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 9, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 13, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 11, SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 12, }},
            {{ SPR_G2_STANDUP_TRACK_QUARTER_LOOP + 10, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kInvertedFlatToDown90QuarterLoopBoundBoxes = {{
        {{
            {{ { { 0, 6, 24 }, { 32, 20, 3 } }, {} }},
            {{ { { -8, 4, 0 }, { 28, 2, 31 } }, { { 0, 0, 48 }, { 32, 20, 1 } } }},
            {{ { { 6, 4, 8 }, { 20, 2, 48 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 2, 63 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 24, 6, 8 }, { 2, 20, 31 } }, {} }},
            {{ { { 24, 6, 8 }, { 2, 20, 63 } }, {} }},
            {{ { { 24, 6, 8 }, { 2, 20, 31 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 0, 24 }, { 20, 32, 3 } }, {} }},
            {{ { { 4, -8, 0 }, { 2, 28, 31 } }, { { 0, 0, 48 }, { 20, 32, 1 } } }},
            {{ { { 4, 6, 8 }, { 2, 20, 48 } }, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            25571,
            25572,
            25571,
            25572,
        }},
        {{
            25573,
            25574,
            25573,
            25574,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBlockBrakesOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Sprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 2, SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 1, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 8, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 9, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Offsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, { 0, 0, 0 } }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
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
            {{ { 16, 16, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25BoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 22, 22, 0 }, { 20, 20, 3 } } }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 34 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 34 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 34 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Sprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 10, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 11, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 19, SPR_G2_STANDUP_TRACK_GENTLE_SMALL_CURVE_BANKED + 18, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Offsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ {}, {} }},
            {{ { 16, 16, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
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
            {{ { 0, 6, 0 }, { 0, 0, 0 } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn3TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn5TileUp25Sprites = {{
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 0,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 1,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 2,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 3,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 4,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 5,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 6,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 7,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 8,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 9,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 11,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 12,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 13,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 14,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 15,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 16,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 17,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 18,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 19,
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
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 20,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 21,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 22,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 23,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 24,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 25,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 26,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 27,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 28,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 29,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 30,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 31,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 32,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 33,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 34,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 35,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 36,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 37,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 38,
            SPR_G2_STANDUP_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 0, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 1, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 2, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 3, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 4, kImageIndexUndefined, }},
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
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 5, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 7, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 8, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 9, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 11, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 12, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 13, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 14, kImageIndexUndefined, }},
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
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 16, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 17, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 18, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 19, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 20,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 21,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 22,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 23,
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
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 24,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 25,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 26,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 27,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 28,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 29,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 30,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 31,
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
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 32,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 33,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 34,
        SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 35,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 44, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 45, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 46, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 47, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 48, kImageIndexUndefined, }},
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
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 49, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 50, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 51, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 52, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 53, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 54, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 55, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 56, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 57, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 58, kImageIndexUndefined, }},
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
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 59, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 60, kImageIndexUndefined, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 61, SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 62, }},
        {{ SPR_G2_STANDUP_TRACK_SLOPE_BANK_TRANSITION + 63, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 1, SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 2, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 4, SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 5, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90BoundBoxes = {{
        {{
            {{ { { 4, 6, 8 }, { 2, 20, 96 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 96 } }, { { 6, 24, 8 }, { 20, 2, 96 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 24, 6, 8 }, { 2, 20, 96 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 96 } }, { { 6, 24, 8 }, { 20, 2, 96 } } }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90Sprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 6, SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 7, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 9, SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 10, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_VERTICAL_TWIST + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 1, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 2, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 3, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 5, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 6, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 8, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 10, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 11, SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 12, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 13, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 15, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 16, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 18, SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 19, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 20, SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 21, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 4 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 4 }, { 32, 20, 0 } }, {} }},
            {{ { { 0, 31, 0 }, { 48, 1, 64 } }, {} }},
            {{ {}, {} }},
            {{ { { 2, 2, 50 }, { 28, 28, 0 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 4 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 48 }, { 20, 32, 3 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 2, 2, 50 }, { 28, 28, 0 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 4 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 30, 20, 3 } }, { { 0, 6, 76 }, { 30, 20, 3 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 2, 2, 50 }, { 24, 60, 0 } }, {} }},
            {{ { { 2, 2, 40 }, { 26, 28, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 4 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 2, 72 }, { 32, 26, 1 } } }},
            {{ { { -16, 2, 0 }, { 48, 26, 1 } }, { { 0, 2, 40 }, { 32, 26, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 22, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 23, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 24, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 25, SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 26, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 27, SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 28, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 29, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 30, SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 31, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 32, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 33, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 34, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 35, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 36, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 37, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 38, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 39, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 40, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 41, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 42, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_CORKSCREW + 43, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 3, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 4, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 5, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 6, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 7, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 8, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 9, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 10, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 11, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 12, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 13, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 14, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 17, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 18, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 19, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 20, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 21, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 22, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 23, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 24, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 0, 2 }, { 32, 32, 0 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 32, 3 } }, { { 0, 0, 140 }, { 32, 32, 3 } } }},
            {{ { { -16, 0, 0 }, { 48, 32, 1 } }, { { 0, 0, 44 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 44 }, { 20, 32, 3 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 68 }, { 20, 32, 3 } } }},
            {{ { { 0, 29, 0 }, { 32, 1, 96 } }, {} }},
            {{ { { 0, 0, 140 }, { 32, 32, 3 } }, {} }},
            {{ { { 0, 0, 44 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 32, 32, 60 }, { 32, 20, 3 } }, {} }},
            {{ { { 31, 0, 0 }, { 1, 32, 96 } }, {} }},
            {{ { { 29, 0, 0 }, { 1, 32, 160 } }, {} }},
            {{ { { 0, 0, 44 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 1, 160 } }, { { 0, 0, 140 }, { 32, 32, 3 } } }},
            {{ { { 0, 0, 44 }, { 32, 32, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 26, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 27, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 28, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 29, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 30, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 31, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 33, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 34, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 35, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 36, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 37, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 38, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 39, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 40, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 41, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 42, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 43, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 44, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 45, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 46, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 47, }},
            {{ SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 48, SPR_G2_STANDUP_TRACK_MEDIUM_HALF_LOOP + 49, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 0, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 1, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 2, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 4, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 5, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 9, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 10, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 11, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 12, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 14, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 15, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 16, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 17, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 32 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 32 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 28 }, { 40, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 31, 0, 0 }, { 1, 40, 32 } } }},
            {{ { { 6, 0, 32 }, { 20, 40, 1 } }, {} }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 56 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 40 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 56 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 40 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 18, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 19, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 20, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 21, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 22, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 23, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 24, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 25, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 26, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 27, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 28, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 29, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 32, SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 33, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 34, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_ZERO_G_ROLL + 35, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 0, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 1, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 2, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 5, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 9, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 10, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 11, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 12, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 13, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 14, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 15, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 17, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 18, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 19, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 96 } } }},
            {{ { { 0, 30, 0 }, { 32, 0, 64 } }, {} }},
            {{ { { 0, 26, 0 }, { 32, 0, 32 } }, {} }},
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
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 72 }, { 32, 20, 3 } } }},
            {{ { { 0, 2, 0 }, { 32, 0, 32 } }, { { 0, 6, 48 }, { 32, 20, 3 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 2, 0, 0 }, { 0, 32, 32 } }, { { 6, 0, 60 }, { 20, 32, 3 } } }},
            {{ { { 29, 0, 0 }, { 1, 32, 40 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 22, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 23, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 24, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 25, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 26, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 27, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 28, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 29, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 30, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 31, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 32, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 33, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 34, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 35, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 36, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 37, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 38, SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 39, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 40, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_LARGE_ZERO_G_ROLL + 41, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25Sprites = {{
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 0,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 1,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 4,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 5,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 8,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 9,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 12,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 13,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 15,
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
            { { 0, -16, 0 }, { 16, 48, 3 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 16,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 17,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 20,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 21,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 24,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 25,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 28,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 29,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 32,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 33,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 34,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 35,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 36,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 38,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 39,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 40,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 41,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 42,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 43,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 44,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 45,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 46,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 47,
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 48,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 49,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 50,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 51,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 52,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 53,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 54,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 55,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 56,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 57,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 58,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 59,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 60,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 61,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 62,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE + 63,
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 0, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 4, kImageIndexUndefined, }},
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 7, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 9, kImageIndexUndefined, }},
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 10, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 14, kImageIndexUndefined, }},
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 17, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 19, kImageIndexUndefined, }},
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 21,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 23,
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
            { { -16, -16, 34 }, { 32, 32, 0 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 24,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 25,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 27,
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
            { { -16, -16, 34 }, { 32, 32, 0 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 29,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 31,
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
            { { -16, -16, 32 }, { 32, 32, 0 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 32,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 33,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 34,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 35,
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
            { { -16, -16, 32 }, { 32, 32, 0 } },
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 44, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 46, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 47, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 48, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 34 }, { 32, 32, 0 } } }},
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 49, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 50, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 51, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 52, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 53, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { -16, -16, 34 }, { 32, 32, 0 } } }},
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 54, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 55, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 56, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 57, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 58, kImageIndexUndefined, }},
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
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 59, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 60, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 61, SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 62, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 63, kImageIndexUndefined, }},
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 36,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 37,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 38,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 39,
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
            { { -16, -16, 42 }, { 32, 32, 0 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 40,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 41,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 42,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 43,
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
            { { -16, -16, 42 }, { 32, 32, 0 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 64,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 65,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 66,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 67,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 68,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 69,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 70,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 71,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 72,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 73,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 74,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 75,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 76,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 77,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 78,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 79,
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
            { { 27, 0, 0 }, { 1, 32, 32 } },
            { { 27, 0, 0 }, { 1, 32, 32 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 80,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 81,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 82,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 83,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 84,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 85,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 86,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 87,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 88,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 89,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 90,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 91,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 92,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 93,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 94,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 95,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 96,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 97,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 98,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 99,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 100,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 101,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 102,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 103,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 104,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 105,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 106,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 107,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 108,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 109,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 110,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 111,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 0, 40 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 0, 27, 0 }, { 32, 1, 32 } },
            { { 0, 27, 0 }, { 32, 1, 32 } },
        }},
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 27, 0, 0 }, { 1, 32, 32 } },
            { { 27, 0, 0 }, { 1, 32, 32 } },
        }},
        {{
            { { 16, 0, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 0, 0 }, { 32, 20, 3 } },
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
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 112,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 113,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 114,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 115,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 116,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 117,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 118,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 119,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 120,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 121,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 122,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 123,
        }},
        {{
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 124,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 125,
            kImageIndexUndefined,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 126,
            SPR_G2_STANDUP_TRACK_GENTLE_LARGE_CURVE_BANKED + 127,
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
            { { 0, 27, 0 }, { 32, 1, 32 } },
            { { 0, 27, 0 }, { 32, 1, 32 } },
        }},
        {{
            { { 0, 0, 40 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 27, 0, 0 }, { 1, 32, 32 } },
            { { 27, 0, 0 }, { 1, 32, 32 } },
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
            SPR_G2_STANDUP_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_STANDUP_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_STANDUP_DIAG_BRAKES + 1,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagBlockBrakesSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_STANDUP_DIAG_BRAKES + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_STANDUP_DIAG_BRAKES + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_STANDUP_DIAG_BRAKES + 3,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_STANDUP_DIAG_BRAKES + 5,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_STANDUP_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_STANDUP_DIAG_BRAKES + 4,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_STANDUP_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_STANDUP_DIAG_BRAKES + 4,
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &kBlockBrakesSprites[0][0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // EndStation
        { &kStationSprites[0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // BeginStation
        { &kStationSprites[0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // MiddleStation
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
        { &kBankedLeftQuarterTurn5TilesSprites[0][0][0], nullptr, &kBankedLeftQuarterTurn5TilesBoundBoxes[0][0][0], false }, // BankedLeftQuarterTurn5Tiles
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
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0][0], &kLeftQuarterTurn5TilesUp25Offsets[0][0], &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0][0], &kRightQuarterTurn5TilesUp25Offsets[0][0], &kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], &kSBendLeftOffsets[0][0], &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], &kSBendRightOffsets[0][0], &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { &kLeftVerticalLoopSprites[0][0], &kLeftVerticalLoopOffsets[0][0], &kLeftVerticalLoopBoundBoxes[0][0], false }, // LeftVerticalLoop
        { &kRightVerticalLoopSprites[0][0], &kRightVerticalLoopOffsets[0][0], &kRightVerticalLoopBoundBoxes[0][0], false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], &kLeftQuarterTurn3TilesOffsets[0][0], &kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &kLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftQuarterTurn3TilesUp25Offsets[0][0], &kLeftQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightQuarterTurn3TilesUp25Offsets[0][0], &kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
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
        { &kFlatToUp60Sprites[0][0], nullptr, &kFlatToUp60BoundBoxes[0][0], false }, // FlatToUp60
        { &kUp60ToFlatSprites[0][0], nullptr, &kUp60ToFlatBoundBoxes[0][0], false }, // Up60ToFlat
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
        { &kBrakesSprites[0], &kBrakesOffsets[0], &kBrakesBoundBoxes[0], false }, // Brakes
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
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // OnRidePhoto
        { nullptr, nullptr, nullptr, true }, // Down25LeftBanked
        { nullptr, nullptr, nullptr, true }, // Down25RightBanked
        { nullptr, nullptr, nullptr, false }, // Watersplash
        { &kFlatToUp60LongBaseSprites[0][0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0][0], false }, // Up60ToFlatLongBase
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
        { &kDiagFlatToUp60Sprites[0][0], &kDiagFlatToUp60Offsets[0][0], &kDiagFlatToUp60BoundBoxes[0][0], false }, // DiagFlatToUp60
        { &kDiagUp60ToFlatSprites[0][0], &kDiagUp60ToFlatOffsets[0][0], &kDiagUp60ToFlatBoundBoxes[0][0], false }, // DiagUp60ToFlat
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
        { &kLeftLargeHalfLoopUpSprites[0][0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0][0], false }, // LeftLargeHalfLoopUp
        { &kRightLargeHalfLoopUpSprites[0][0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0][0], false }, // RightLargeHalfLoopUp
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
        { &kUp90ToInvertedFlatQuarterLoopSprites[0][0][0], nullptr, &kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &kInvertedFlatToDown90QuarterLoopSprites[0][0][0], nullptr, &kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0][0], false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0], &kBlockBrakesOffsets[0], &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
        { &kLeftBankedQuarterTurn3TileUp25Sprites[0][0][0], &kLeftBankedQuarterTurn3TileUp25Offsets[0][0][0], &kLeftBankedQuarterTurn3TileUp25BoundBoxes[0][0][0], false }, // LeftBankedQuarterTurn3TileUp25
        { &kRightBankedQuarterTurn3TileUp25Sprites[0][0][0], &kRightBankedQuarterTurn3TileUp25Offsets[0][0][0], &kRightBankedQuarterTurn3TileUp25BoundBoxes[0][0][0], false }, // RightBankedQuarterTurn3TileUp25
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
        { &kLeftLargeCorkscrewUpSprites[0][0][0], nullptr, &kLeftLargeCorkscrewUpBoundBoxes[0][0][0], false }, // LeftLargeCorkscrewUp
        { &kRightLargeCorkscrewUpSprites[0][0][0], nullptr, &kRightLargeCorkscrewUpBoundBoxes[0][0][0], false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { &kLeftMediumHalfLoopUpSprites[0][0][0], nullptr, &kLeftMediumHalfLoopUpBoundBoxes[0][0][0], false }, // LeftMediumHalfLoopUp
        { &kRightMediumHalfLoopUpSprites[0][0][0], nullptr, &kRightMediumHalfLoopUpBoundBoxes[0][0][0], false }, // RightMediumHalfLoopUp
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
} // namespace OpenRCT2::StandUpRollerCoaster
