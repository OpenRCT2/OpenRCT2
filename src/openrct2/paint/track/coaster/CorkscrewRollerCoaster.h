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

namespace OpenRCT2::CorkscrewRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            16224,
            16225,
            16224,
            16225,
        }},
        {{
            16226,
            16227,
            16228,
            16229,
        }},
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
        16236,
        16237,
        16236,
        16237,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        { { 0, 6, 3 }, { 32, 20, 1 } },
        { { 6, 0, 3 }, { 20, 32, 1 } },
        { { 0, 6, 3 }, { 32, 20, 1 } },
        { { 6, 0, 3 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            16286,
            16287,
            16288,
            16289,
        }},
        {{
            16314,
            16315,
            16316,
            16317,
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
            16302,
            16303,
            16304,
            16305,
        }},
        {{
            16330,
            16331,
            16332,
            16333,
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
            16278,
            16279,
            16280,
            16281,
        }},
        {{
            16306,
            16307,
            16308,
            16309,
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
            {{ 16290, kImageIndexUndefined, }},
            {{ 16291, 16294, }},
            {{ 16292, 16295, }},
            {{ 16293, kImageIndexUndefined, }},
        }},
        {{
            {{ 16318, kImageIndexUndefined, }},
            {{ 16319, 16322, }},
            {{ 16320, 16323, }},
            {{ 16321, kImageIndexUndefined, }},
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
            {{ 16296, kImageIndexUndefined, }},
            {{ 16297, 16300, }},
            {{ 16298, 16301, }},
            {{ 16299, kImageIndexUndefined, }},
        }},
        {{
            {{ 16324, kImageIndexUndefined, }},
            {{ 16325, 16328, }},
            {{ 16326, 16329, }},
            {{ 16327, kImageIndexUndefined, }},
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
            16282,
            16283,
            16284,
            16285,
        }},
        {{
            16310,
            16311,
            16312,
            16313,
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
            16343,
            kImageIndexUndefined,
            16342,
            16341,
            kImageIndexUndefined,
            16340,
            16339,
        }},
        {{
            16348,
            kImageIndexUndefined,
            16347,
            16346,
            kImageIndexUndefined,
            16345,
            16344,
        }},
        {{
            16353,
            kImageIndexUndefined,
            16352,
            16351,
            kImageIndexUndefined,
            16350,
            16349,
        }},
        {{
            16338,
            kImageIndexUndefined,
            16337,
            16336,
            kImageIndexUndefined,
            16335,
            16334,
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
        {{ 16238, 16246, }},
        {{ 16239, 16247, }},
        {{ 16240, kImageIndexUndefined, }},
        {{ 16241, kImageIndexUndefined, }},
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
        {{ 16242, kImageIndexUndefined, }},
        {{ 16243, kImageIndexUndefined, }},
        {{ 16244, 16248, }},
        {{ 16245, 16249, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 16363, 16374, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16362, kImageIndexUndefined, }},
            {{ 16361, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16360, kImageIndexUndefined, }},
            {{ 16359, kImageIndexUndefined, }},
        }},
        {{
            {{ 16368, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16367, kImageIndexUndefined, }},
            {{ 16366, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16365, kImageIndexUndefined, }},
            {{ 16364, kImageIndexUndefined, }},
        }},
        {{
            {{ 16373, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16372, kImageIndexUndefined, }},
            {{ 16371, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16370, kImageIndexUndefined, }},
            {{ 16369, 16375, }},
        }},
        {{
            {{ 16358, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16357, kImageIndexUndefined, }},
            {{ 16356, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16355, kImageIndexUndefined, }},
            {{ 16354, kImageIndexUndefined, }},
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
        {{ 16250, 16254, }},
        {{ 16251, 16255, }},
        {{ 16252, kImageIndexUndefined, }},
        {{ 16253, kImageIndexUndefined, }},
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
        {{ 16256, kImageIndexUndefined, }},
        {{ 16257, kImageIndexUndefined, }},
        {{ 16258, 16260, }},
        {{ 16259, 16261, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 16262, 16266, }},
        {{ 16263, 16267, }},
        {{ 16264, kImageIndexUndefined, }},
        {{ 16265, kImageIndexUndefined, }},
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
        {{ 16268, kImageIndexUndefined, }},
        {{ 16269, kImageIndexUndefined, }},
        {{ 16270, 16272, }},
        {{ 16271, 16273, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        16274,
        16275,
        16276,
        16277,
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
            16510,
            kImageIndexUndefined,
            16511,
            16512,
            kImageIndexUndefined,
            16513,
            16514,
        }},
        {{
            16515,
            kImageIndexUndefined,
            16516,
            16517,
            kImageIndexUndefined,
            16518,
            16519,
        }},
        {{
            16520,
            kImageIndexUndefined,
            16521,
            16522,
            kImageIndexUndefined,
            16523,
            16524,
        }},
        {{
            16525,
            kImageIndexUndefined,
            16526,
            16527,
            kImageIndexUndefined,
            16528,
            16529,
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
            16490,
            kImageIndexUndefined,
            16491,
            16492,
            kImageIndexUndefined,
            16493,
            16494,
        }},
        {{
            16495,
            kImageIndexUndefined,
            16496,
            16497,
            kImageIndexUndefined,
            16498,
            16499,
        }},
        {{
            16500,
            kImageIndexUndefined,
            16501,
            16502,
            kImageIndexUndefined,
            16503,
            16504,
        }},
        {{
            16505,
            kImageIndexUndefined,
            16506,
            16507,
            kImageIndexUndefined,
            16508,
            16509,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            16402,
            16403,
            16404,
            16405,
        }},
        {{
            16406,
            16407,
            16408,
            16409,
        }},
        {{
            16405,
            16404,
            16403,
            16402,
        }},
        {{
            16409,
            16408,
            16407,
            16406,
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
            16410,
            16411,
            16412,
            16413,
        }},
        {{
            16414,
            16415,
            16416,
            16417,
        }},
        {{
            16413,
            16412,
            16411,
            16410,
        }},
        {{
            16417,
            16416,
            16415,
            16414,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            16562,
            16563,
            16564,
            16565,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16566,
            16567,
            16568,
            16569,
        }},
        {{
            16570,
            16571,
            16572,
            16573,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16574,
            16575,
            16576,
            16577,
        }},
        {{
            16569,
            16568,
            16567,
            16566,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16565,
            16564,
            16563,
            16562,
        }},
        {{
            16577,
            16576,
            16575,
            16574,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16573,
            16572,
            16571,
            16570,
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
            { { 16, 0, 0 }, { 3, 16, 119 } },
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
            { { 14, 0, 0 }, { 2, 32, 63 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 0, 0, 32 }, { 16, 32, 3 } },
            {},
            {},
            { { 16, 0, 32 }, { 16, 32, 3 } },
            { { 16, 16, 0 }, { 16, 2, 119 } },
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
            { { 16, 0, 0 }, { 3, 16, 119 } },
            { { 0, 0, 0 }, { 32, 26, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 7 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 16, 16, 0 }, { 16, 2, 119 } },
            { { 16, 0, 32 }, { 16, 32, 3 } },
            {},
            {},
            { { 0, 0, 32 }, { 16, 32, 3 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 14, 0, 0 }, { 2, 32, 63 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopSprites = {{
        {{
            16593,
            16592,
            16591,
            16590,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16589,
            16588,
            16587,
            16586,
        }},
        {{
            16585,
            16584,
            16583,
            16582,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16581,
            16580,
            16579,
            16578,
        }},
        {{
            16586,
            16587,
            16588,
            16589,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16590,
            16591,
            16592,
            16593,
        }},
        {{
            16578,
            16579,
            16580,
            16581,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16582,
            16583,
            16584,
            16585,
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
            16381,
            kImageIndexUndefined,
            16380,
            16379,
        }},
        {{
            16384,
            kImageIndexUndefined,
            16383,
            16382,
        }},
        {{
            16387,
            kImageIndexUndefined,
            16386,
            16385,
        }},
        {{
            16378,
            kImageIndexUndefined,
            16377,
            16376,
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
            {{ 16393, 16400, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16392, kImageIndexUndefined, }},
            {{ 16391, kImageIndexUndefined, }},
        }},
        {{
            {{ 16396, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16395, kImageIndexUndefined, }},
            {{ 16394, kImageIndexUndefined, }},
        }},
        {{
            {{ 16399, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16398, kImageIndexUndefined, }},
            {{ 16397, 16401, }},
        }},
        {{
            {{ 16390, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16389, kImageIndexUndefined, }},
            {{ 16388, kImageIndexUndefined, }},
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
            16541,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16540,
        }},
        {{
            16543,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16542,
        }},
        {{
            16545,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16544,
        }},
        {{
            16539,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16538,
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
            16530,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16531,
        }},
        {{
            16532,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16533,
        }},
        {{
            16534,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16535,
        }},
        {{
            16536,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16537,
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
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpSprites = {{
        {{
            26972,
            26973,
            26974,
        }},
        {{
            26978,
            26979,
            26980,
        }},
        {{
            26971,
            26970,
            26969,
        }},
        {{
            26977,
            26976,
            26975,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 8 }, { 32, 20, 3 } },
            { { 0, 6, -8 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 8 }, { 20, 32, 3 } },
            { { 6, 0, -8 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 8 }, { 32, 20, 3 } },
            { { 0, 6, -8 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 8 }, { 20, 32, 3 } },
            { { 6, 0, -8 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpSprites = {{
        {{
            26984,
            26985,
            26986,
        }},
        {{
            26990,
            26991,
            26992,
        }},
        {{
            26983,
            26982,
            26981,
        }},
        {{
            26989,
            26988,
            26987,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftTwistDownToUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kHalfLoopUpSprites = {{
        {{
            16594,
            16595,
            16596,
            16597,
        }},
        {{
            16602,
            16603,
            16604,
            16605,
        }},
        {{
            16601,
            16600,
            16599,
            16598,
        }},
        {{
            16609,
            16608,
            16607,
            16606,
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
            16610,
            16611,
            16612,
        }},
        {{
            16613,
            16614,
            16615,
        }},
        {{
            16616,
            16617,
            16618,
        }},
        {{
            16619,
            16620,
            16621,
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
            16622,
            16623,
            16624,
        }},
        {{
            16625,
            16626,
            16627,
        }},
        {{
            16628,
            16629,
            16630,
        }},
        {{
            16631,
            16632,
            16633,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp60Sprites = {{
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 2, SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 1, }},
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 4, SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 3, }},
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
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
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 6, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 7, SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 8, }},
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 10, SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 9, }},
        {{ SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 11, kImageIndexUndefined, }},
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
            {{ 16481, 16488, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16480, kImageIndexUndefined, }},
            {{ 16479, kImageIndexUndefined, }},
            {{ 16478, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16477, kImageIndexUndefined, }},
            {{ 16476, kImageIndexUndefined, }},
        }},
        {{
            {{ 16484, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16483, kImageIndexUndefined, }},
            {{ 16482, kImageIndexUndefined, }},
            {{ 16481, 16488, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16480, kImageIndexUndefined, }},
            {{ 16479, kImageIndexUndefined, }},
        }},
        {{
            {{ 16487, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16486, kImageIndexUndefined, }},
            {{ 16485, 16489, }},
            {{ 16484, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16483, kImageIndexUndefined, }},
            {{ 16482, kImageIndexUndefined, }},
        }},
        {{
            {{ 16478, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16477, kImageIndexUndefined, }},
            {{ 16476, kImageIndexUndefined, }},
            {{ 16487, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16486, kImageIndexUndefined, }},
            {{ 16485, 16489, }},
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
            {{ 16462, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16463, kImageIndexUndefined, }},
            {{ 16464, kImageIndexUndefined, }},
            {{ 16465, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16466, kImageIndexUndefined, }},
            {{ 16467, 16474, }},
        }},
        {{
            {{ 16465, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16466, kImageIndexUndefined, }},
            {{ 16467, 16474, }},
            {{ 16468, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16469, kImageIndexUndefined, }},
            {{ 16470, kImageIndexUndefined, }},
        }},
        {{
            {{ 16468, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16469, kImageIndexUndefined, }},
            {{ 16470, kImageIndexUndefined, }},
            {{ 16471, 16475, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16472, kImageIndexUndefined, }},
            {{ 16473, kImageIndexUndefined, }},
        }},
        {{
            {{ 16471, 16475, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16472, kImageIndexUndefined, }},
            {{ 16473, kImageIndexUndefined, }},
            {{ 16462, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16463, kImageIndexUndefined, }},
            {{ 16464, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 16449, 16460, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16448, kImageIndexUndefined, }},
            {{ 16447, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16446, kImageIndexUndefined, }},
            {{ 16445, kImageIndexUndefined, }},
            {{ 16444, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16443, kImageIndexUndefined, }},
            {{ 16442, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16441, kImageIndexUndefined, }},
            {{ 16440, kImageIndexUndefined, }},
        }},
        {{
            {{ 16454, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16453, kImageIndexUndefined, }},
            {{ 16452, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16451, kImageIndexUndefined, }},
            {{ 16450, kImageIndexUndefined, }},
            {{ 16449, 16460, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16448, kImageIndexUndefined, }},
            {{ 16447, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16446, kImageIndexUndefined, }},
            {{ 16445, kImageIndexUndefined, }},
        }},
        {{
            {{ 16459, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16458, kImageIndexUndefined, }},
            {{ 16457, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16456, kImageIndexUndefined, }},
            {{ 16455, 16461, }},
            {{ 16454, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16453, kImageIndexUndefined, }},
            {{ 16452, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16451, kImageIndexUndefined, }},
            {{ 16450, kImageIndexUndefined, }},
        }},
        {{
            {{ 16444, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16443, kImageIndexUndefined, }},
            {{ 16442, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16441, kImageIndexUndefined, }},
            {{ 16440, kImageIndexUndefined, }},
            {{ 16459, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16458, kImageIndexUndefined, }},
            {{ 16457, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16456, kImageIndexUndefined, }},
            {{ 16455, 16461, }},
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
            {{ 16418, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16419, kImageIndexUndefined, }},
            {{ 16420, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16421, kImageIndexUndefined, }},
            {{ 16422, kImageIndexUndefined, }},
            {{ 16423, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16424, kImageIndexUndefined, }},
            {{ 16425, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16426, kImageIndexUndefined, }},
            {{ 16427, 16438, }},
        }},
        {{
            {{ 16423, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16424, kImageIndexUndefined, }},
            {{ 16425, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16426, kImageIndexUndefined, }},
            {{ 16427, 16438, }},
            {{ 16428, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16429, kImageIndexUndefined, }},
            {{ 16430, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16431, kImageIndexUndefined, }},
            {{ 16432, kImageIndexUndefined, }},
        }},
        {{
            {{ 16428, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16429, kImageIndexUndefined, }},
            {{ 16430, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16431, kImageIndexUndefined, }},
            {{ 16432, kImageIndexUndefined, }},
            {{ 16433, 16439, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16434, kImageIndexUndefined, }},
            {{ 16435, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16436, kImageIndexUndefined, }},
            {{ 16437, kImageIndexUndefined, }},
        }},
        {{
            {{ 16433, 16439, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16434, kImageIndexUndefined, }},
            {{ 16435, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16436, kImageIndexUndefined, }},
            {{ 16437, kImageIndexUndefined, }},
            {{ 16418, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16419, kImageIndexUndefined, }},
            {{ 16420, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16421, kImageIndexUndefined, }},
            {{ 16422, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ 16555, 16559, }},
        {{ 16556, 16560, }},
        {{ 16557, 16561, }},
        {{ 16554, 16558, }},
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
        {{ 16546, 16550, }},
        {{ 16547, 16551, }},
        {{ 16548, 16552, }},
        {{ 16549, 16553, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        16230,
        16231,
        16230,
        16231,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBoosterSprites = {{
        {{ SPR_G2_CORKSCREW_BOOSTER_NE_SW_1 + 0, SPR_G2_CORKSCREW_BOOSTER_NE_SW_2 + 0, }},
        {{ SPR_G2_CORKSCREW_BOOSTER_NW_SE_1 + 0, SPR_G2_CORKSCREW_BOOSTER_NW_SE_2 + 0, }},
        {{ SPR_G2_CORKSCREW_BOOSTER_NE_SW_1 + 0, SPR_G2_CORKSCREW_BOOSTER_NE_SW_2 + 0, }},
        {{ SPR_G2_CORKSCREW_BOOSTER_NW_SE_1 + 0, SPR_G2_CORKSCREW_BOOSTER_NW_SE_2 + 0, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBoosterBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25LeftBankedSprites = {{
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 36,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 37,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 38,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 39,
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
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 40,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 41,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 42,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 43,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            16788,
            16789,
            16790,
            16791,
        }},
        {{
            16792,
            16793,
            16794,
            16795,
        }},
        {{
            16796,
            16797,
            16798,
            16799,
        }},
        {{
            16800,
            16801,
            16802,
            16803,
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
            16804,
            16805,
            16806,
            16807,
        }},
        {{
            16808,
            16809,
            16810,
            16811,
        }},
        {{
            16812,
            16813,
            16814,
            16815,
        }},
        {{
            16816,
            16817,
            16818,
            16819,
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
            26607,
            kImageIndexUndefined,
        }},
        {{
            26608,
            kImageIndexUndefined,
        }},
        {{
            26605,
            kImageIndexUndefined,
        }},
        {{
            26606,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp90Offsets = {{
        {{
            { 0, 0, 8 }, 
            {},
        }},
        {{
            { 0, 0, 8 }, 
            {},
        }},
        {{
            { 0, 0, 8 }, 
            {},
        }},
        {{
            { 0, 0, 8 }, 
            {},
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
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 1,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 2,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 3,
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
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 2,
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 3,
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 0,
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 1,
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
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 4,
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 5,
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 6,
        SPR_G2_CORKSCREW_TRACK_VERTICAL + 7,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp90ToUp60BoundBoxes = {{
        { { 4, 6, 8 }, { 2, 20, 48 } },
        { { 6, 39, 8 }, { 20, 2, 31 } },
        { { 39, 6, 8 }, { 2, 20, 31 } },
        { { 6, 4, 8 }, { 20, 2, 48 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Sprites = {{
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 6,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 7,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 4,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_VERTICAL + 5,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90BoundBoxes = {{
        {{
            { { 0, 27, 0 }, { 32, 1, 80 } },
            {},
        }},
        {{
            { { 6, 0, 2 }, { 20, 6, 3 } },
            {},
        }},
        {{
            { { 0, 6, 2 }, { 6, 20, 3 } },
            {},
        }},
        {{
            { { 27, 0, 0 }, { 1, 32, 80 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            16740,
            16741,
            16742,
            kImageIndexUndefined,
            16743,
        }},
        {{
            16744,
            16745,
            16746,
            kImageIndexUndefined,
            16747,
        }},
        {{
            16748,
            16749,
            16750,
            kImageIndexUndefined,
            16751,
        }},
        {{
            16752,
            16753,
            16754,
            kImageIndexUndefined,
            16755,
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
            16724,
            16725,
            16726,
            kImageIndexUndefined,
            16727,
        }},
        {{
            16728,
            16729,
            16730,
            kImageIndexUndefined,
            16731,
        }},
        {{
            16732,
            16733,
            16734,
            kImageIndexUndefined,
            16735,
        }},
        {{
            16736,
            16737,
            16738,
            kImageIndexUndefined,
            16739,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            16772,
            16773,
            16774,
            kImageIndexUndefined,
            16775,
        }},
        {{
            16776,
            16777,
            16778,
            kImageIndexUndefined,
            16779,
        }},
        {{
            16780,
            16781,
            16782,
            kImageIndexUndefined,
            16783,
        }},
        {{
            16784,
            16785,
            16786,
            kImageIndexUndefined,
            16787,
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
            16756,
            16757,
            16758,
            kImageIndexUndefined,
            16759,
        }},
        {{
            16760,
            16761,
            16762,
            kImageIndexUndefined,
            16763,
        }},
        {{
            16764,
            16765,
            16766,
            kImageIndexUndefined,
            16767,
        }},
        {{
            16768,
            16769,
            16770,
            kImageIndexUndefined,
            16771,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                16634,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16635,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16636,
                kImageIndexUndefined,
            }},
            {{
                16637,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16696,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16697,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16698,
                kImageIndexUndefined,
            }},
            {{
                16699,
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
                16646,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16647,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16648,
                kImageIndexUndefined,
            }},
            {{
                16649,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16708,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16709,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16710,
                kImageIndexUndefined,
            }},
            {{
                16711,
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
                16658,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16659,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16660,
                kImageIndexUndefined,
            }},
            {{
                16661,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16720,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16721,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16722,
                kImageIndexUndefined,
            }},
            {{
                16723,
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
                16638,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16639,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16640,
                kImageIndexUndefined,
            }},
            {{
                16641,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16700,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16701,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16702,
                kImageIndexUndefined,
            }},
            {{
                16703,
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
                16650,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16651,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16652,
                kImageIndexUndefined,
            }},
            {{
                16653,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16712,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16713,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16714,
                kImageIndexUndefined,
            }},
            {{
                16715,
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
                16654,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16655,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16656,
                kImageIndexUndefined,
            }},
            {{
                16657,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16716,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16717,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16718,
                kImageIndexUndefined,
            }},
            {{
                16719,
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
                16642,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16643,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16644,
                kImageIndexUndefined,
            }},
            {{
                16645,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                16704,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                16705,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                16706,
                kImageIndexUndefined,
            }},
            {{
                16707,
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
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 13,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 14,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 15,
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
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 18,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_SMALL_FLAT_TO_STEEP + 19,
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
            {{ 16666, 16670, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16667, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16668, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 16669, kImageIndexUndefined, }},
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
            {{ 16671, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16672, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16673, 16675, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 16674, kImageIndexUndefined, }},
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
            {{ 16686, 16690, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16687, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16688, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 16689, kImageIndexUndefined, }},
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
            {{ 16691, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16692, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16693, 16695, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 16694, kImageIndexUndefined, }},
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
            {{ 16676, 16680, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16677, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16678, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 16679, kImageIndexUndefined, }},
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
            {{ 16681, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16682, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 16683, 16685, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 16684, kImageIndexUndefined, }},
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
            16662,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            16663,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            16664,
            kImageIndexUndefined,
        }},
        {{
            16665,
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
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 0, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 1, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 2, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 3, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 4, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 5, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 6, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 7, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 8, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 9, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 10, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 12, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 13, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 14, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 15, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 16, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 17, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 18, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 19, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 20, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 21, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 22, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 23, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 24, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 25, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 26, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 27, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 28, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 29, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 30, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 31, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 32, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 33, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 34, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 35, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 36, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 37, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 38, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 39, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 40, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 41, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 42, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 43, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 44, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 45, }},
            {{ SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 46, SPR_G2_CORKSCREW_TRACK_BARREL_ROLL + 47, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBarrelRollUpToDownBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 1, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 2, SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 4, SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 20, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 18, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 8, kImageIndexUndefined, }},
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
            {{ { { 0, 6, -6 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 6, 0, -6 }, { 32, 20, 3 } }, { { 6, 0, 32 }, { 32, 20, 3 } } }},
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
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 6, 0, -6 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 19, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 13, SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 14, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 15, SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 21, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 16, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_TURN_BANK_TRANSITION + 17, kImageIndexUndefined, }},
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
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 6, 0, 0 }, { 6, 0, 0 } }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 3, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 5, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 56, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 8, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 57, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 9, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 58, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 10, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 11, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 12, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 13, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 14, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 16, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 59, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 17, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 18, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 19, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 20, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 21, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 22, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 23, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 24, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 26, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 27, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 3 } }, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, { { 0, 16, 200 }, { 32, 16, 3 } } }},
            {{ { { 0, 16, 32 }, { 32, 16, 0 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 48 }, { 20, 32, 3 } } }},
            {{ { { 0, 0, 0 }, { 16, 32, 3 } }, { { 0, 0, 70 }, { 16, 32, 3 } } }},
            {{ { { 0, 0, 200 }, { 16, 32, 0 } }, {} }},
            {{ { { 16, 0, 110 }, { 16, 16, 0 } }, {} }},
            {{ { { 16, 0, 200 }, { 16, 32, 0 } }, {} }},
            {{ { { 16, 0, 32 }, { 16, 32, 0 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 3 } }, { { 0, 16, 76 }, { 32, 16, 3 } } }},
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
            {{ { { 0, 0, 0 }, { 16, 32, 3 } }, {} }},
            {{ { { 0, 0, 32 }, { 16, 32, 0 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 28, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 29, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 30, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 31, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 33, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 35, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 36, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 37, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 60, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 38, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 39, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 40, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 41, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 42, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 43, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 61, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 44, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 62, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 45, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 46, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 47, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 48, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 49, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 50, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 51, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 52, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 53, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 54, SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 63, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_HALF_LOOP + 55, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistUpSprites = {{
        {{
            26972,
            26973,
            26974,
        }},
        {{
            26978,
            26979,
            26980,
        }},
        {{
            26971,
            26970,
            26969,
        }},
        {{
            26977,
            26976,
            26975,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistUpOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistUpBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistUpSprites = {{
        {{
            26984,
            26985,
            26986,
        }},
        {{
            26990,
            26991,
            26992,
        }},
        {{
            26983,
            26982,
            26981,
        }},
        {{
            26989,
            26988,
            26987,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistUpOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftFlyerTwistUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedUpSprites = {{
        {{
            16594,
            16595,
            16596,
            16597,
        }},
        {{
            16602,
            16603,
            16604,
            16605,
        }},
        {{
            16601,
            16600,
            16599,
            16598,
        }},
        {{
            16609,
            16608,
            16607,
            16606,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedUpOffsets = {{
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
    kFlyerHalfLoopUninvertedUpBoundBoxes = {{
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
    kLeftFlyerCorkscrewUpSprites = {{
        {{
            16610,
            16611,
            16612,
        }},
        {{
            16613,
            16614,
            16615,
        }},
        {{
            16616,
            16617,
            16618,
        }},
        {{
            16619,
            16620,
            16621,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerCorkscrewUpBoundBoxes = {{
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
    kRightFlyerCorkscrewUpSprites = {{
        {{
            16622,
            16623,
            16624,
        }},
        {{
            16625,
            16626,
            16627,
        }},
        {{
            16628,
            16629,
            16630,
        }},
        {{
            16631,
            16632,
            16633,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftFlyerCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 1, SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 12, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 3, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 6, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 9, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 10, SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 13, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 11, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopBoundBoxes = {{
        {{
            {{ { { 4, 6, 8 }, { 2, 20, 31 } }, {} }},
            {{ { { -8, 6, 0 }, { 2, 20, 31 } }, { { 0, 0, 48 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 24 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 3, 63 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
        }},
        {{
            {{ { { 24, 6, 8 }, { 2, 20, 31 } }, {} }},
            {{ { { 24, 6, 8 }, { 2, 20, 63 } }, {} }},
            {{ { { 24, 6, 8 }, { 2, 20, 31 } }, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 31 } }, {} }},
            {{ { { 6, -8, 0 }, { 20, 2, 31 } }, { { 0, 0, 48 }, { 20, 32, 1 } } }},
            {{ { { 6, 0, 24 }, { 20, 32, 3 } }, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kInvertedFlatToDown90QuarterLoopSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 1, SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 12, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 5, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 11, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 10, SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 13, }},
            {{ SPR_G2_CORKSCREW_TRACK_QUARTER_LOOP + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kInvertedFlatToDown90QuarterLoopBoundBoxes = {{
        {{
            {{ { { 0, 6, 24 }, { 32, 20, 3 } }, {} }},
            {{ { { -8, 6, 0 }, { 2, 20, 31 } }, { { 0, 0, 48 }, { 32, 20, 1 } } }},
            {{ { { 4, 6, 8 }, { 2, 20, 31 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 24, 8 }, { 20, 2, 31 } }, {} }},
            {{ { { 6, 24, 8 }, { 20, 3, 63 } }, {} }},
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
            {{ { { 6, -8, 0 }, { 20, 2, 31 } }, { { 0, 0, 48 }, { 20, 32, 1 } } }},
            {{ { { 6, 4, 8 }, { 20, 2, 31 } }, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            16232,
            16233,
            16232,
            16233,
        }},
        {{
            16234,
            16235,
            16234,
            16235,
        }},
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 1,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 3,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 5,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 6,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 16,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 7,
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
            { 16, 16, 0 }, 
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
            { { 16, 16, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Sprites = {{
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 8,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 17,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 9,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 11,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 13,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_SMALL_CURVE_BANKED + 15,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 16, 16, 0 }, 
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 0,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 1,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 2,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 3,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 4,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 5,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 6,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 7,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 8,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 9,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 11,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 12,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 13,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 14,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 15,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 16,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 17,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 18,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 19,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 20,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 21,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 22,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 23,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 24,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 25,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 26,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 27,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 28,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 29,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 30,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 31,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 32,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 33,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 34,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 35,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 36,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 37,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 38,
            SPR_G2_CORKSCREW_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 0, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 1, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 2, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 3, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 4, kImageIndexUndefined, }},
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
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 5, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 7, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 8, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 9, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 11, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 12, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 13, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 14, kImageIndexUndefined, }},
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
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 16, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 17, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 18, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 19, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 20,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 21,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 22,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 23,
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
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 24,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 25,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 26,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 27,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 28,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 29,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 30,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 31,
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
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 32,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 33,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 34,
        SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 35,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 44, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 45, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 46, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 47, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 48, kImageIndexUndefined, }},
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
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 49, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 50, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 51, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 52, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 53, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 54, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 55, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 56, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 57, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 58, kImageIndexUndefined, }},
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
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 59, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 60, kImageIndexUndefined, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 61, SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 62, }},
        {{ SPR_G2_CORKSCREW_TRACK_SLOPE_BANK_TRANSITION + 63, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 1, SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 2, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 4, SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 5, }},
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
            {{ { { 24, 24, 8 }, { 2, 2, 63 } }, {} }},
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
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 6, SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 7, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 9, SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 10, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_VERTICAL_TWIST + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 0, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 1, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 2, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 3, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 5, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 6, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 8, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 10, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 13, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 15, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 16, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 17, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 18, SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 40, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 19, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 4 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 31, 0 }, { 48, 1, 64 } }, {} }},
            {{ {}, {} }},
            {{ { { 2, 2, 50 }, { 28, 28, 0 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 4 }, { 20, 32, 3 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 2, 2, 50 }, { 28, 28, 0 } }, {} }},
            {{ { { 2, 2, 40 }, { 28, 28, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 4 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 20, 20, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 50 }, { 26, 32, 0 } }, {} }},
            {{ { { 2, 2, 40 }, { 26, 28, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 4 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 64 }, { 32, 32, 1 } } }},
            {{ { { 2, 2, 44 }, { 28, 26, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 20, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 21, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 22, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 23, SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 41, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 25, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 26, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 28, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 29, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 30, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 31, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 32, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 33, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 35, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 36, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 37, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 38, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_CORKSCREW + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 3, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 40, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 5, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 41, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 6, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 42, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 10, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 11, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 12, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 13, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 17, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 18, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 43, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 19, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 0, 2 }, { 32, 32, 0 } }, {} }},
            {{ { { 0, 2, 0 }, { 32, 1, 160 } }, { { 0, 29, 0 }, { 32, 1, 160 } } }},
            {{ { { 0, 2, 48 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 0, 0, 32 }, { 20, 32, 3 } } }},
            {{ { { 31, 0, 0 }, { 32, 0, 64 } }, { { 0, 0, 64 }, { 20, 32, 3 } } }},
            {{ { { 0, 29, 0 }, { 32, 1, 96 } }, {} }},
            {{ { { 0, 0, 140 }, { 32, 32, 3 } }, {} }},
            {{ { { 2, 0, 48 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 32, 32, 60 }, { 32, 20, 3 } }, {} }},
            {{ { { 31, 0, 0 }, { 1, 32, 96 } }, {} }},
            {{ { { 29, 0, 0 }, { 1, 32, 160 } }, {} }},
            {{ { { 0, 0, 48 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 1, 160 } }, { { 0, 0, 140 }, { 20, 32, 1 } } }},
            {{ { { 0, 0, 48 }, { 32, 32, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 20, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 21, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 22, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 23, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 44, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 26, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 27, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 28, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 29, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 30, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 45, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 31, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 46, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 33, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 35, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 36, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 37, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 38, SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 47, }},
            {{ SPR_G2_CORKSCREW_TRACK_MEDIUM_HALF_LOOP + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 0, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 1, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 5, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 6, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 7, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 9, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 11, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 14, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 15, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 17, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 18, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 19, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 40 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 40 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 22 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 31, 0, 0 }, { 1, 32, 32 } } }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 48 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 48 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 22, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 23, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 24, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 25, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 26, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 27, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 28, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 29, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 33, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 34, SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 35, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 37, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_ZERO_G_ROLL + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 0, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 1, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 2, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 3, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 4, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 10, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 15, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 16, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 17, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 18, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 19, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 20, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 21, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 22, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 23, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 24, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 25, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 26, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 27, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 28, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 96 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 64 } } }},
            {{ { { 0, 26, 0 }, { 32, 0, 20 } }, {} }},
        }},
        {{
            {{ { { 30, 0, 0 }, { 0, 32, 96 } }, {} }},
            {{ { { 30, 0, 0 }, { 0, 32, 64 } }, {} }},
            {{ { { 30, 0, 0 }, { 0, 32, 48 } }, {} }},
            {{ { { 26, 0, 0 }, { 0, 32, 20 } }, {} }},
        }},
        {{
            {{ { { 1, 6, 0 }, { 30, 20, 16 } }, { { 0, 30, 0 }, { 32, 0, 40 } } }},
            {{ { { -3, 0, 0 }, { 40, 8, 64 } }, { { 0, 30, 0 }, { 32, 0, 64 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 56 } } }},
            {{ { { 0, 10, 0 }, { 32, 0, 28 } }, { { 0, 30, 0 }, { 32, 0, 28 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 2, 0, 0 }, { 0, 32, 32 } }, { { 29, 0, 32 }, { 1, 32, 32 } } }},
            {{ { { 2, 0, 0 }, { 0, 32, 32 } }, { { 29, 0, 0 }, { 1, 32, 40 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 29, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 31, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 32, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 33, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 34, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 35, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 36, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 37, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 38, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 39, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 40, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 41, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 42, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 44, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 46, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 48, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 50, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 51, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 52, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 53, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 54, SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 55, }},
            {{ SPR_G2_CORKSCREW_TRACK_LARGE_ZERO_G_ROLL + 57, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedDownSprites = {{
        {{
            27076,
            27075,
            27074,
            27073,
        }},
        {{
            27084,
            27083,
            27082,
            27081,
        }},
        {{
            27077,
            27078,
            27079,
            27080,
        }},
        {{
            27085,
            27086,
            27087,
            27088,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedDownOffsets = {{
        {{
            { 0, 0, 29 }, 
            { 16, 0, -3 }, 
            { 0, 0, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 0, 0, 29 }, 
            { 0, 12, -3 }, 
            { 14, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
        {{
            { 0, 16, 29 }, 
            { 10, 16, -3 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 16, 0, 29 }, 
            { 16, 16, -3 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedDownBoundBoxes = {{
        {{
            { { 0, 6, 34 }, { 32, 20, 3 } },
            { { 16, 29, 2 }, { 5, 2, 119 } },
            { { 0, 29, 29 }, { 32, 2, 3 } },
            { { 0, 6, 45 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 0, -6 }, { 2, 2, 3 } },
            { { -6, 0, 2 }, { 2, 32, 119 } },
            { { 29, 0, 29 }, { 2, 32, 63 } },
            { { 6, 0, 45 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 0, -6 }, { 32, 2, 3 } },
            { { 10, 0, 2 }, { 4, 2, 119 } },
            { { 0, 0, 29 }, { 32, 2, 3 } },
            { { 0, 6, 45 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 34 }, { 20, 32, 3 } },
            { { 29, 29, 2 }, { 2, 2, 119 } },
            { { 20, 29, 29 }, { 2, 2, 3 } },
            { { 6, 0, 45 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25Sprites = {{
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 0,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 1,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 4,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 5,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 8,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 9,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 12,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 13,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 15,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 16,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 17,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 20,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 21,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 24,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 25,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 28,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 29,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 32,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 33,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 34,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 35,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 36,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 38,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 39,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 40,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 41,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 42,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 43,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 44,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 45,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 46,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 47,
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
            { { 0, 6, 0 }, { 32, 20, 10 } },
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 48,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 49,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 50,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 51,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 52,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 53,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 54,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 55,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 56,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 57,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 58,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 59,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 60,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 61,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 62,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE + 63,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 10 } },
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 0, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 4, kImageIndexUndefined, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 7, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 9, kImageIndexUndefined, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 10, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 14, kImageIndexUndefined, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 17, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 19, kImageIndexUndefined, }},
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 21,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 23,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 24,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 25,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 27,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 29,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 31,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 32,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 33,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 34,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 35,
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 44, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 46, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 47, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 48, kImageIndexUndefined, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 49, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 50, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 51, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 52, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 53, kImageIndexUndefined, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 54, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 55, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 56, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 57, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 58, kImageIndexUndefined, }},
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
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 59, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 60, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 61, SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 62, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 63, kImageIndexUndefined, }},
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 36,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 37,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 38,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 39,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 40,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 41,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 42,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 43,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 64,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 65,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 66,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 67,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 68,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 69,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 70,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 71,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 72,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 73,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 74,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 75,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 76,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 77,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 78,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 79,
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 80,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 81,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 82,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 83,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 84,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 85,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 86,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 87,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 88,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 89,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 90,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 91,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 92,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 93,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 94,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 95,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 96,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 97,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 98,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 99,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 100,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 101,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 102,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 103,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 104,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 105,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 106,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 107,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 108,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 109,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 110,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 111,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 0, 40 }, { 32, 32, 1 } },
            { { 0, 0, 32 }, { 32, 32, 1 } },
            {},
            { { 0, 31, 32 }, { 32, 1, 32 } },
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
            { { 0, 0, 0 }, { 32, 16, 10 } },
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
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 112,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 113,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 114,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 115,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 116,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 117,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 118,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 119,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 120,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 121,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 122,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 123,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 124,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 125,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 126,
            SPR_G2_CORKSCREW_TRACK_GENTLE_LARGE_CURVE_BANKED + 127,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 10 } },
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
            { { 0, 0, 40 }, { 32, 32, 1 } },
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
            SPR_G2_CORKSCREW_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_DIAG_BRAKES + 1,
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
                SPR_G2_CORKSCREW_DIAG_BRAKES + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_CORKSCREW_DIAG_BRAKES + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_CORKSCREW_DIAG_BRAKES + 3,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_CORKSCREW_DIAG_BRAKES + 5,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_CORKSCREW_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_CORKSCREW_DIAG_BRAKES + 4,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_CORKSCREW_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_CORKSCREW_DIAG_BRAKES + 4,
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
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kDiagFlatToUp60LongBaseSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 3,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 5,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 7,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 9,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 11,
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
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 13,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 15,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 18,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 19,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 20,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 21,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 22,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_FLAT_TO_STEEP_DIAGONAL + 23,
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
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 0,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 1,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 2,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 3,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 5,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 6,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 7,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 8,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 9,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 10,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 11,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 13,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 14,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 15,
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
            { { 0, 6, 28 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, 48 }, { 32, 32, 1 } },
            {},
            {},
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 6, 0, 28 }, { 20, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            { { 2, 2, 6 }, { 28, 28, 1 } },
            { { 0, 0, 0 }, { 32, 1, 64 } },
            { { 0, 6, 28 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, -8 }, { 32, 32, 1 } },
            {},
            {},
            { { 2, 2, 12 }, { 28, 28, 1 } },
            { { 6, 0, 58 }, { 20, 32, 1 } },
            { { 6, 0, 28 }, { 20, 32, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kRightEighthDiveLoopUpToOrthogonalSprites = {{
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 16,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 17,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 18,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 19,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 21,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 22,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 23,
        }},
        {{
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 24,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 25,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 26,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 27,
        }},
        {{
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 29,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 30,
            SPR_G2_CORKSCREW_TRACK_DIVE_LOOP + 31,
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false }, // Flat
        { &kBlockBrakesSprites[0][0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false }, // BeginStation
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false }, // MiddleStation
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
        { &kLeftVerticalLoopSprites[0][0], &kLeftVerticalLoopOffsets[0][0], &kLeftVerticalLoopBoundBoxes[0][0], false }, // LeftVerticalLoop
        { &kRightVerticalLoopSprites[0][0], &kRightVerticalLoopOffsets[0][0], &kRightVerticalLoopBoundBoxes[0][0], false }, // RightVerticalLoop
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
        { &kLeftTwistDownToUpSprites[0][0], &kLeftTwistDownToUpOffsets[0][0], &kLeftTwistDownToUpBoundBoxes[0][0], false }, // LeftTwistDownToUp
        { &kRightTwistDownToUpSprites[0][0], &kRightTwistDownToUpOffsets[0][0], &kRightTwistDownToUpBoundBoxes[0][0], false }, // RightTwistDownToUp
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
        { &kBrakesSprites[0], nullptr, &kBrakesBoundBoxes[0], false }, // Brakes
        { &kBoosterSprites[0][0], nullptr, &kBoosterBoundBoxes[0][0], false }, // Booster
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
        { &kUp90Sprites[0][0], &kUp90Offsets[0][0], &kUp90BoundBoxes[0][0], false }, // Up90
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
        { &kLeftFlyerTwistUpSprites[0][0], &kLeftFlyerTwistUpOffsets[0][0], &kLeftFlyerTwistUpBoundBoxes[0][0], false }, // LeftFlyerTwistUp
        { &kRightFlyerTwistUpSprites[0][0], &kRightFlyerTwistUpOffsets[0][0], &kRightFlyerTwistUpBoundBoxes[0][0], false }, // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, true }, // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, true }, // RightFlyerTwistDown
        { &kFlyerHalfLoopUninvertedUpSprites[0][0], &kFlyerHalfLoopUninvertedUpOffsets[0][0], &kFlyerHalfLoopUninvertedUpBoundBoxes[0][0], false }, // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopInvertedDown
        { &kLeftFlyerCorkscrewUpSprites[0][0], nullptr, &kLeftFlyerCorkscrewUpBoundBoxes[0][0], false }, // LeftFlyerCorkscrewUp
        { &kRightFlyerCorkscrewUpSprites[0][0], nullptr, &kRightFlyerCorkscrewUpBoundBoxes[0][0], false }, // RightFlyerCorkscrewUp
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
        { &kBlockBrakesSprites[0][0], nullptr, &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
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
        { &kFlyerHalfLoopUninvertedDownSprites[0][0], &kFlyerHalfLoopUninvertedDownOffsets[0][0], &kFlyerHalfLoopUninvertedDownBoundBoxes[0][0], false }, // FlyerHalfLoopUninvertedDown
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
        { &kDiagFlatToUp60LongBaseSprites[0][0], &kDiagFlatToUp60LongBaseOffsets[0][0], &kDiagFlatToUp60LongBaseBoundBoxes[0][0], false }, // DiagFlatToUp60LongBase
        { &kDiagUp60ToFlatLongBaseSprites[0][0], &kDiagUp60ToFlatLongBaseOffsets[0][0], &kDiagUp60ToFlatLongBaseBoundBoxes[0][0], false }, // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlatLongBase
        { &kLeftEighthDiveLoopUpToOrthogonalSprites[0][0], &kLeftEighthDiveLoopUpToOrthogonalOffsets[0][0], &kLeftEighthDiveLoopUpToOrthogonalBoundBoxes[0][0], false }, // LeftEighthDiveLoopUpToOrthogonal
        { &kRightEighthDiveLoopUpToOrthogonalSprites[0][0], &kRightEighthDiveLoopUpToOrthogonalOffsets[0][0], &kRightEighthDiveLoopUpToOrthogonalBoundBoxes[0][0], false }, // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, true }, // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, true }, // RightEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, false }, // DiagDown25Brakes
    } };
    // clang-format on
} // namespace OpenRCT2::CorkscrewRollerCoaster
