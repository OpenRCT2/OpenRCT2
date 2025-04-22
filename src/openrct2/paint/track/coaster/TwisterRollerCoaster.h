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

namespace OpenRCT2::TwisterRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            17146,
            17147,
            17146,
            17147,
        }},
        {{
            17486,
            17487,
            17488,
            17489,
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
        17154,
        17155,
        17154,
        17155,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kStationBoundBoxes = { {
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
    } };

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            17204,
            17205,
            17206,
            17207,
        }},
        {{
            17498,
            17499,
            17500,
            17501,
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
            17220,
            17221,
            17222,
            17223,
        }},
        {{
            17514,
            17515,
            17516,
            17517,
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
            17196,
            17197,
            17198,
            17199,
        }},
        {{
            17490,
            17491,
            17492,
            17493,
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
            {{ 17208, kImageIndexUndefined, }},
            {{ 17209, 17212, }},
            {{ 17210, 17213, }},
            {{ 17211, kImageIndexUndefined, }},
        }},
        {{
            {{ 17502, kImageIndexUndefined, }},
            {{ 17503, 17506, }},
            {{ 17504, 17507, }},
            {{ 17505, kImageIndexUndefined, }},
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
            {{ 17214, kImageIndexUndefined, }},
            {{ 17215, 17218, }},
            {{ 17216, 17219, }},
            {{ 17217, kImageIndexUndefined, }},
        }},
        {{
            {{ 17508, kImageIndexUndefined, }},
            {{ 17509, 17512, }},
            {{ 17510, 17513, }},
            {{ 17511, kImageIndexUndefined, }},
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
            17200,
            17201,
            17202,
            17203,
        }},
        {{
            17494,
            17495,
            17496,
            17497,
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
            17259,
            kImageIndexUndefined,
            17258,
            17257,
            kImageIndexUndefined,
            17256,
            17255,
        }},
        {{
            17264,
            kImageIndexUndefined,
            17263,
            17262,
            kImageIndexUndefined,
            17261,
            17260,
        }},
        {{
            17269,
            kImageIndexUndefined,
            17268,
            17267,
            kImageIndexUndefined,
            17266,
            17265,
        }},
        {{
            17254,
            kImageIndexUndefined,
            17253,
            17252,
            kImageIndexUndefined,
            17251,
            17250,
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
        {{ 17156, 17164, }},
        {{ 17157, 17165, }},
        {{ 17158, kImageIndexUndefined, }},
        {{ 17159, kImageIndexUndefined, }},
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
        {{ 17160, kImageIndexUndefined, }},
        {{ 17161, kImageIndexUndefined, }},
        {{ 17162, 17166, }},
        {{ 17163, 17167, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 17279, 17290, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17278, kImageIndexUndefined, }},
            {{ 17277, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17276, kImageIndexUndefined, }},
            {{ 17275, kImageIndexUndefined, }},
        }},
        {{
            {{ 17284, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17283, kImageIndexUndefined, }},
            {{ 17282, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17281, kImageIndexUndefined, }},
            {{ 17280, kImageIndexUndefined, }},
        }},
        {{
            {{ 17289, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17288, kImageIndexUndefined, }},
            {{ 17287, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17286, kImageIndexUndefined, }},
            {{ 17285, 17291, }},
        }},
        {{
            {{ 17274, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17273, kImageIndexUndefined, }},
            {{ 17272, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17271, kImageIndexUndefined, }},
            {{ 17270, kImageIndexUndefined, }},
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
        {{ 17168, 17172, }},
        {{ 17169, 17173, }},
        {{ 17170, kImageIndexUndefined, }},
        {{ 17171, kImageIndexUndefined, }},
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
        {{ 17174, kImageIndexUndefined, }},
        {{ 17175, kImageIndexUndefined, }},
        {{ 17176, 17178, }},
        {{ 17177, 17179, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 17180, 17184, }},
        {{ 17181, 17185, }},
        {{ 17182, kImageIndexUndefined, }},
        {{ 17183, kImageIndexUndefined, }},
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
        {{ 17186, kImageIndexUndefined, }},
        {{ 17187, kImageIndexUndefined, }},
        {{ 17188, 17190, }},
        {{ 17189, 17191, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        17192,
        17193,
        17194,
        17195,
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
            17344,
            kImageIndexUndefined,
            17345,
            17346,
            kImageIndexUndefined,
            17347,
            17348,
        }},
        {{
            17349,
            kImageIndexUndefined,
            17350,
            17351,
            kImageIndexUndefined,
            17352,
            17353,
        }},
        {{
            17354,
            kImageIndexUndefined,
            17355,
            17356,
            kImageIndexUndefined,
            17357,
            17358,
        }},
        {{
            17359,
            kImageIndexUndefined,
            17360,
            17361,
            kImageIndexUndefined,
            17362,
            17363,
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
            17324,
            kImageIndexUndefined,
            17325,
            17326,
            kImageIndexUndefined,
            17327,
            17328,
        }},
        {{
            17329,
            kImageIndexUndefined,
            17330,
            17331,
            kImageIndexUndefined,
            17332,
            17333,
        }},
        {{
            17334,
            kImageIndexUndefined,
            17335,
            17336,
            kImageIndexUndefined,
            17337,
            17338,
        }},
        {{
            17339,
            kImageIndexUndefined,
            17340,
            17341,
            kImageIndexUndefined,
            17342,
            17343,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            17308,
            17309,
            17310,
            17311,
        }},
        {{
            17312,
            17313,
            17314,
            17315,
        }},
        {{
            17311,
            17310,
            17309,
            17308,
        }},
        {{
            17315,
            17314,
            17313,
            17312,
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
            17316,
            17317,
            17318,
            17319,
        }},
        {{
            17320,
            17321,
            17322,
            17323,
        }},
        {{
            17319,
            17318,
            17317,
            17316,
        }},
        {{
            17323,
            17322,
            17321,
            17320,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            17594,
            17595,
            17596,
            17597,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17598,
            17599,
            17600,
            17601,
        }},
        {{
            17602,
            17603,
            17604,
            17605,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17606,
            17607,
            17608,
            17609,
        }},
        {{
            17601,
            17600,
            17599,
            17598,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17597,
            17596,
            17595,
            17594,
        }},
        {{
            17609,
            17608,
            17607,
            17606,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17605,
            17604,
            17603,
            17602,
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
            17625,
            17624,
            17623,
            17622,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17621,
            17620,
            17619,
            17618,
        }},
        {{
            17617,
            17616,
            17615,
            17614,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17613,
            17612,
            17611,
            17610,
        }},
        {{
            17618,
            17619,
            17620,
            17621,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17622,
            17623,
            17624,
            17625,
        }},
        {{
            17610,
            17611,
            17612,
            17613,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17614,
            17615,
            17616,
            17617,
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
            17229,
            kImageIndexUndefined,
            17228,
            17227,
        }},
        {{
            17232,
            kImageIndexUndefined,
            17231,
            17230,
        }},
        {{
            17235,
            kImageIndexUndefined,
            17234,
            17233,
        }},
        {{
            17226,
            kImageIndexUndefined,
            17225,
            17224,
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
            {{ 17241, 17248, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17240, kImageIndexUndefined, }},
            {{ 17239, kImageIndexUndefined, }},
        }},
        {{
            {{ 17244, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17243, kImageIndexUndefined, }},
            {{ 17242, kImageIndexUndefined, }},
        }},
        {{
            {{ 17247, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17246, kImageIndexUndefined, }},
            {{ 17245, 17249, }},
        }},
        {{
            {{ 17238, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17237, kImageIndexUndefined, }},
            {{ 17236, kImageIndexUndefined, }},
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
            17375,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17374,
        }},
        {{
            17377,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17376,
        }},
        {{
            17379,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17378,
        }},
        {{
            17373,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17372,
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
            17364,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17365,
        }},
        {{
            17366,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17367,
        }},
        {{
            17368,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17369,
        }},
        {{
            17370,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17371,
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
            27430,
            27431,
            27432,
        }},
        {{
            27436,
            27437,
            27438,
        }},
        {{
            27429,
            27428,
            27427,
        }},
        {{
            27435,
            27434,
            27433,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpOffsets = {{
        {{
            { 0, 6, -5 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
        {{
            { 0, 6, -5 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpBoundBoxes = {{
        {{
            { { 0, 0, 6 }, { 32, 20, 3 } },
            { { 0, 6, 8 }, { 32, 20, 3 } },
            { { 0, 6, -8 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 0, 6 }, { 20, 32, 3 } },
            { { 6, 0, 8 }, { 20, 32, 3 } },
            { { 6, 0, -8 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 0, 6 }, { 32, 20, 3 } },
            { { 0, 6, 8 }, { 32, 20, 3 } },
            { { 0, 6, -8 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 0, 6 }, { 20, 32, 3 } },
            { { 6, 0, 8 }, { 20, 32, 3 } },
            { { 6, 0, -8 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpSprites = {{
        {{
            27442,
            27443,
            27444,
        }},
        {{
            27448,
            27449,
            27450,
        }},
        {{
            27441,
            27440,
            27439,
        }},
        {{
            27447,
            27446,
            27445,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpOffsets = {{
        {{
            { 0, 6, -5 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
        {{
            { 0, 6, -5 }, 
            { 0, 6, 8 }, 
            { 0, 6, -8 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 8 }, 
            { 6, 0, -8 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftTwistDownToUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kHalfLoopUpSprites = {{
        {{
            17626,
            17627,
            17628,
            17629,
        }},
        {{
            17634,
            17635,
            17636,
            17637,
        }},
        {{
            17633,
            17632,
            17631,
            17630,
        }},
        {{
            17641,
            17640,
            17639,
            17638,
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
            17690,
            17691,
            17692,
        }},
        {{
            17693,
            17694,
            17695,
        }},
        {{
            17696,
            17697,
            17698,
        }},
        {{
            17699,
            17700,
            17701,
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
            17702,
            17703,
            17704,
        }},
        {{
            17705,
            17706,
            17707,
        }},
        {{
            17708,
            17709,
            17710,
        }},
        {{
            17711,
            17712,
            17713,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatToUp60Sprites = {{
        {{
            {{ 17452, kImageIndexUndefined, }},
            {{ 17453, 17456, }},
            {{ 17454, 17457, }},
            {{ 17455, kImageIndexUndefined, }},
        }},
        {{
            {{ 17464, kImageIndexUndefined, }},
            {{ 17465, 17468, }},
            {{ 17466, 17469, }},
            {{ 17467, kImageIndexUndefined, }},
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
            {{ 17458, kImageIndexUndefined, }},
            {{ 17459, 17462, }},
            {{ 17460, 17463, }},
            {{ 17461, kImageIndexUndefined, }},
        }},
        {{
            {{ 17470, kImageIndexUndefined, }},
            {{ 17471, 17474, }},
            {{ 17472, 17475, }},
            {{ 17473, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToFlatBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 4 } }, {} }},
        {{ { { 4, 29, 2 }, { 24, 1, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 29, 4, 2 }, { 1, 24, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 4 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 17399, 17406, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17398, kImageIndexUndefined, }},
            {{ 17397, kImageIndexUndefined, }},
            {{ 17396, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17395, kImageIndexUndefined, }},
            {{ 17394, kImageIndexUndefined, }},
        }},
        {{
            {{ 17402, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17401, kImageIndexUndefined, }},
            {{ 17400, kImageIndexUndefined, }},
            {{ 17399, 17406, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17398, kImageIndexUndefined, }},
            {{ 17397, kImageIndexUndefined, }},
        }},
        {{
            {{ 17405, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17404, kImageIndexUndefined, }},
            {{ 17403, 17407, }},
            {{ 17402, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17401, kImageIndexUndefined, }},
            {{ 17400, kImageIndexUndefined, }},
        }},
        {{
            {{ 17396, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17395, kImageIndexUndefined, }},
            {{ 17394, kImageIndexUndefined, }},
            {{ 17405, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17404, kImageIndexUndefined, }},
            {{ 17403, 17407, }},
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
            {{ 17380, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17381, kImageIndexUndefined, }},
            {{ 17382, kImageIndexUndefined, }},
            {{ 17383, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17384, kImageIndexUndefined, }},
            {{ 17385, 17392, }},
        }},
        {{
            {{ 17383, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17384, kImageIndexUndefined, }},
            {{ 17385, 17392, }},
            {{ 17386, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17387, kImageIndexUndefined, }},
            {{ 17388, kImageIndexUndefined, }},
        }},
        {{
            {{ 17386, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17387, kImageIndexUndefined, }},
            {{ 17388, kImageIndexUndefined, }},
            {{ 17389, 17393, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17390, kImageIndexUndefined, }},
            {{ 17391, kImageIndexUndefined, }},
        }},
        {{
            {{ 17389, 17393, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17390, kImageIndexUndefined, }},
            {{ 17391, kImageIndexUndefined, }},
            {{ 17380, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17381, kImageIndexUndefined, }},
            {{ 17382, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 17439, 17450, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17438, kImageIndexUndefined, }},
            {{ 17437, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17436, kImageIndexUndefined, }},
            {{ 17435, kImageIndexUndefined, }},
            {{ 17434, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17433, kImageIndexUndefined, }},
            {{ 17432, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17431, kImageIndexUndefined, }},
            {{ 17430, kImageIndexUndefined, }},
        }},
        {{
            {{ 17444, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17443, kImageIndexUndefined, }},
            {{ 17442, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17441, kImageIndexUndefined, }},
            {{ 17440, kImageIndexUndefined, }},
            {{ 17439, 17450, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17438, kImageIndexUndefined, }},
            {{ 17437, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17436, kImageIndexUndefined, }},
            {{ 17435, kImageIndexUndefined, }},
        }},
        {{
            {{ 17449, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17448, kImageIndexUndefined, }},
            {{ 17447, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17446, kImageIndexUndefined, }},
            {{ 17445, 17451, }},
            {{ 17444, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17443, kImageIndexUndefined, }},
            {{ 17442, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17441, kImageIndexUndefined, }},
            {{ 17440, kImageIndexUndefined, }},
        }},
        {{
            {{ 17434, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17433, kImageIndexUndefined, }},
            {{ 17432, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17431, kImageIndexUndefined, }},
            {{ 17430, kImageIndexUndefined, }},
            {{ 17449, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17448, kImageIndexUndefined, }},
            {{ 17447, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17446, kImageIndexUndefined, }},
            {{ 17445, 17451, }},
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
            {{ 17408, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17409, kImageIndexUndefined, }},
            {{ 17410, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17411, kImageIndexUndefined, }},
            {{ 17412, kImageIndexUndefined, }},
            {{ 17413, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17414, kImageIndexUndefined, }},
            {{ 17415, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17416, kImageIndexUndefined, }},
            {{ 17417, 17428, }},
        }},
        {{
            {{ 17413, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17414, kImageIndexUndefined, }},
            {{ 17415, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17416, kImageIndexUndefined, }},
            {{ 17417, 17428, }},
            {{ 17418, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17419, kImageIndexUndefined, }},
            {{ 17420, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17421, kImageIndexUndefined, }},
            {{ 17422, kImageIndexUndefined, }},
        }},
        {{
            {{ 17418, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17419, kImageIndexUndefined, }},
            {{ 17420, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17421, kImageIndexUndefined, }},
            {{ 17422, kImageIndexUndefined, }},
            {{ 17423, 17429, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17424, kImageIndexUndefined, }},
            {{ 17425, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17426, kImageIndexUndefined, }},
            {{ 17427, kImageIndexUndefined, }},
        }},
        {{
            {{ 17423, 17429, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17424, kImageIndexUndefined, }},
            {{ 17425, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17426, kImageIndexUndefined, }},
            {{ 17427, kImageIndexUndefined, }},
            {{ 17408, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17409, kImageIndexUndefined, }},
            {{ 17410, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17411, kImageIndexUndefined, }},
            {{ 17412, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ 17301, 17305, }},
        {{ 17302, 17306, }},
        {{ 17303, 17307, }},
        {{ 17300, 17304, }},
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
        {{ 17292, 17296, }},
        {{ 17293, 17297, }},
        {{ 17294, 17298, }},
        {{ 17295, 17299, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        17148,
        17149,
        17148,
        17149,
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
        SPR_G2_BM_BOOSTER_NE_SW + 0,
        SPR_G2_BM_BOOSTER_NW_SE + 0,
        SPR_G2_BM_BOOSTER_NE_SW + 0,
        SPR_G2_BM_BOOSTER_NW_SE + 0,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBoosterBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25LeftBankedSprites = {{
        17914,
        17915,
        17916,
        17917,
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
        17918,
        17919,
        17920,
        17921,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            18030,
            18031,
            18032,
            18033,
        }},
        {{
            18034,
            18035,
            18036,
            18037,
        }},
        {{
            18038,
            18039,
            18040,
            18041,
        }},
        {{
            18042,
            18043,
            18044,
            18045,
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
            18046,
            18047,
            18048,
            18049,
        }},
        {{
            18050,
            18051,
            18052,
            18053,
        }},
        {{
            18054,
            18055,
            18056,
            18057,
        }},
        {{
            18058,
            18059,
            18060,
            18061,
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
            17526,
            kImageIndexUndefined,
        }},
        {{
            17527,
            kImageIndexUndefined,
        }},
        {{
            17528,
            kImageIndexUndefined,
        }},
        {{
            17529,
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
            17518,
            kImageIndexUndefined,
        }},
        {{
            17519,
            kImageIndexUndefined,
        }},
        {{
            17520,
            kImageIndexUndefined,
        }},
        {{
            17521,
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
        17520,
        17521,
        17518,
        17519,
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
        17522,
        17523,
        17524,
        17525,
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
            17524,
            kImageIndexUndefined,
        }},
        {{
            17525,
            kImageIndexUndefined,
        }},
        {{
            17522,
            kImageIndexUndefined,
        }},
        {{
            17523,
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
            { { 6, 0, 8 }, { 20, 32, 3 } },
            {},
        }},
        {{
            { { 0, 6, 8 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 6, 24, 8 }, { 20, 2, 31 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBrakeForDropSprites = {{
        {{ 17482, 17485, }},
        {{ 17483, kImageIndexUndefined, }},
        {{ 17480, kImageIndexUndefined, }},
        {{ 17481, 17484, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBrakeForDropBoundBoxes = {{
        {{ { { 29, 4, 2 }, { 1, 24, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 4 } }, {} }},
        {{ { { 0, 2, 0 }, { 32, 27, 4 } }, {} }},
        {{ { { 4, 29, 2 }, { 24, 1, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            17546,
            17547,
            17548,
            kImageIndexUndefined,
            17549,
        }},
        {{
            17550,
            17551,
            17552,
            kImageIndexUndefined,
            17553,
        }},
        {{
            17554,
            17555,
            17556,
            kImageIndexUndefined,
            17557,
        }},
        {{
            17558,
            17559,
            17560,
            kImageIndexUndefined,
            17561,
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
            17530,
            17531,
            17532,
            kImageIndexUndefined,
            17533,
        }},
        {{
            17534,
            17535,
            17536,
            kImageIndexUndefined,
            17537,
        }},
        {{
            17538,
            17539,
            17540,
            kImageIndexUndefined,
            17541,
        }},
        {{
            17542,
            17543,
            17544,
            kImageIndexUndefined,
            17545,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            17578,
            17579,
            17580,
            kImageIndexUndefined,
            17581,
        }},
        {{
            17582,
            17583,
            17584,
            kImageIndexUndefined,
            17585,
        }},
        {{
            17586,
            17587,
            17588,
            kImageIndexUndefined,
            17589,
        }},
        {{
            17590,
            17591,
            17592,
            kImageIndexUndefined,
            17593,
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
            17562,
            17563,
            17564,
            kImageIndexUndefined,
            17565,
        }},
        {{
            17566,
            17567,
            17568,
            kImageIndexUndefined,
            17569,
        }},
        {{
            17570,
            17571,
            17572,
            kImageIndexUndefined,
            17573,
        }},
        {{
            17574,
            17575,
            17576,
            kImageIndexUndefined,
            17577,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                17788,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17789,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17790,
                kImageIndexUndefined,
            }},
            {{
                17791,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17858,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17859,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17860,
                kImageIndexUndefined,
            }},
            {{
                17861,
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
                17800,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17801,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17802,
                kImageIndexUndefined,
            }},
            {{
                17803,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17870,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17871,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17872,
                kImageIndexUndefined,
            }},
            {{
                17873,
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
                17812,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17813,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17814,
                kImageIndexUndefined,
            }},
            {{
                17815,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17882,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17883,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17884,
                kImageIndexUndefined,
            }},
            {{
                17885,
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
                17792,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17793,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17794,
                kImageIndexUndefined,
            }},
            {{
                17795,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17862,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17863,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17864,
                kImageIndexUndefined,
            }},
            {{
                17865,
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
                17804,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17805,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17806,
                kImageIndexUndefined,
            }},
            {{
                17807,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17874,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17875,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17876,
                kImageIndexUndefined,
            }},
            {{
                17877,
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
                17808,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17809,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17810,
                kImageIndexUndefined,
            }},
            {{
                17811,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17878,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17879,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17880,
                kImageIndexUndefined,
            }},
            {{
                17881,
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
                17796,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17797,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17798,
                kImageIndexUndefined,
            }},
            {{
                17799,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17866,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17867,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17868,
                kImageIndexUndefined,
            }},
            {{
                17869,
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
                17816,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17817,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17818,
                kImageIndexUndefined,
            }},
            {{
                17819,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17886,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17887,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17888,
                kImageIndexUndefined,
            }},
            {{
                17889,
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
                17820,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17821,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17822,
                kImageIndexUndefined,
            }},
            {{
                17823,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                17890,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                17891,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                17892,
                kImageIndexUndefined,
            }},
            {{
                17893,
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
            {{ 17828, 17832, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17829, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17830, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17831, kImageIndexUndefined, }},
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
            {{ 17833, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17834, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17835, 17837, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17836, kImageIndexUndefined, }},
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
            {{ 17848, 17852, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17849, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17850, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17851, kImageIndexUndefined, }},
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
            {{ 17853, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17854, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17855, 17857, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17856, kImageIndexUndefined, }},
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
            {{ 17838, 17842, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17839, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17840, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17841, kImageIndexUndefined, }},
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
            {{ 17843, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17844, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17845, 17847, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17846, kImageIndexUndefined, }},
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
            17824,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            17825,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            17826,
            kImageIndexUndefined,
        }},
        {{
            17827,
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
            {{ 17642, 17666, }},
            {{ 17643, 17667, }},
            {{ 17644, 17668, }},
        }},
        {{
            {{ 17645, 17669, }},
            {{ 17646, 17670, }},
            {{ 17647, 17671, }},
        }},
        {{
            {{ 17648, 17672, }},
            {{ 17649, 17673, }},
            {{ 17650, 17674, }},
        }},
        {{
            {{ 17651, 17675, }},
            {{ 17652, 17676, }},
            {{ 17653, 17677, }},
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
            {{ 17654, 17678, }},
            {{ 17655, 17679, }},
            {{ 17656, 17680, }},
        }},
        {{
            {{ 17657, 17681, }},
            {{ 17658, 17682, }},
            {{ 17659, 17683, }},
        }},
        {{
            {{ 17660, 17684, }},
            {{ 17661, 17685, }},
            {{ 17662, 17686, }},
        }},
        {{
            {{ 17663, 17687, }},
            {{ 17664, 17688, }},
            {{ 17665, 17689, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBarrelRollUpToDownBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ 17725, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17724, kImageIndexUndefined, }},
        }},
        {{
            {{ 17727, 17731, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17726, kImageIndexUndefined, }},
        }},
        {{
            {{ 17729, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17728, kImageIndexUndefined, }},
        }},
        {{
            {{ 17723, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17722, kImageIndexUndefined, }},
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
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
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
            {{ {}, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ 17714, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17715, kImageIndexUndefined, }},
        }},
        {{
            {{ 17716, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17717, kImageIndexUndefined, }},
        }},
        {{
            {{ 17718, 17730, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17719, kImageIndexUndefined, }},
        }},
        {{
            {{ 17720, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 17721, kImageIndexUndefined, }},
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kPoweredLiftSprites = {{
        17476,
        17477,
        17478,
        17479,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kPoweredLiftBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            17732,
            17733,
            17734,
            17735,
            17736,
            17737,
            17738,
        }},
        {{
            17739,
            17740,
            17741,
            17742,
            17743,
            17744,
            17745,
        }},
        {{
            17746,
            17747,
            17748,
            17749,
            17750,
            17751,
            17752,
        }},
        {{
            17753,
            17754,
            17755,
            17756,
            17757,
            17758,
            17759,
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
            17781,
            17782,
            17783,
            17784,
            17785,
            17786,
            17787,
        }},
        {{
            17774,
            17775,
            17776,
            17777,
            17778,
            17779,
            17780,
        }},
        {{
            17767,
            17768,
            17769,
            17770,
            17771,
            17772,
            17773,
        }},
        {{
            17760,
            17761,
            17762,
            17763,
            17764,
            17765,
            17766,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistUpSprites = {{
        {{
            27430,
            27431,
            27432,
        }},
        {{
            27436,
            27437,
            27438,
        }},
        {{
            27429,
            27428,
            27427,
        }},
        {{
            27435,
            27434,
            27433,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistUpOffsets = {{
        {{
            { 0, 6, -5 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
        {{
            { 0, 6, -5 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistUpBoundBoxes = {{
        {{
            { { 0, 6, 6 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 6 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 6 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 6 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistUpSprites = {{
        {{
            27442,
            27443,
            27444,
        }},
        {{
            27448,
            27449,
            27450,
        }},
        {{
            27441,
            27440,
            27439,
        }},
        {{
            27447,
            27446,
            27445,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistUpOffsets = {{
        {{
            { 0, 6, -5 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
        {{
            { 0, 6, -5 }, 
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
        }},
        {{
            { 6, 0, -5 }, 
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftFlyerTwistUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedUpSprites = {{
        {{
            17626,
            17627,
            17628,
            17629,
        }},
        {{
            17634,
            17635,
            17636,
            17637,
        }},
        {{
            17633,
            17632,
            17631,
            17630,
        }},
        {{
            17641,
            17640,
            17639,
            17638,
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
    kUp90ToInvertedFlatQuarterLoopSprites = {{
        {{
            18062,
            18063,
            18064,
        }},
        {{
            18065,
            18066,
            18067,
        }},
        {{
            18068,
            18069,
            18070,
        }},
        {{
            18071,
            18072,
            18073,
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
            18064,
            18063,
            18062,
            kImageIndexUndefined,
        }},
        {{
            18067,
            18066,
            18065,
            kImageIndexUndefined,
        }},
        {{
            18070,
            18069,
            18068,
            kImageIndexUndefined,
        }},
        {{
            18073,
            18072,
            18071,
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            17150,
            17151,
            17150,
            17151,
        }},
        {{
            17152,
            17153,
            17152,
            17153,
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
            18025,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18024,
        }},
        {{
            18027,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18026,
        }},
        {{
            18029,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18028,
        }},
        {{
            18023,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18022,
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
            18014,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18015,
        }},
        {{
            18016,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18017,
        }},
        {{
            18018,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18019,
        }},
        {{
            18020,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18021,
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
            17978,
            kImageIndexUndefined,
            17979,
            17980,
            kImageIndexUndefined,
            17981,
            17982,
        }},
        {{
            17983,
            kImageIndexUndefined,
            17984,
            17985,
            kImageIndexUndefined,
            17986,
            17987,
        }},
        {{
            17988,
            kImageIndexUndefined,
            17989,
            17990,
            kImageIndexUndefined,
            17991,
            17992,
        }},
        {{
            17993,
            kImageIndexUndefined,
            17994,
            17995,
            kImageIndexUndefined,
            17996,
            17997,
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
            17958,
            kImageIndexUndefined,
            17959,
            17960,
            kImageIndexUndefined,
            17961,
            17962,
        }},
        {{
            17963,
            kImageIndexUndefined,
            17964,
            17965,
            kImageIndexUndefined,
            17966,
            17967,
        }},
        {{
            17968,
            kImageIndexUndefined,
            17969,
            17970,
            kImageIndexUndefined,
            17971,
            17972,
        }},
        {{
            17973,
            kImageIndexUndefined,
            17974,
            17975,
            kImageIndexUndefined,
            17976,
            17977,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ 17922, kImageIndexUndefined, }},
        {{ 17923, 17930, }},
        {{ 17924, kImageIndexUndefined, }},
        {{ 17925, kImageIndexUndefined, }},
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
        {{ 17926, kImageIndexUndefined, }},
        {{ 17927, kImageIndexUndefined, }},
        {{ 17928, 17931, }},
        {{ 17929, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ 17932, kImageIndexUndefined, }},
        {{ 17933, 17940, }},
        {{ 17934, kImageIndexUndefined, }},
        {{ 17935, kImageIndexUndefined, }},
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
        {{ 17936, kImageIndexUndefined, }},
        {{ 17937, kImageIndexUndefined, }},
        {{ 17938, 17941, }},
        {{ 17939, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        17942,
        17943,
        17944,
        17945,
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
        17946,
        17947,
        17948,
        17949,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        17950,
        17951,
        17952,
        17953,
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
        17954,
        17955,
        17956,
        17957,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ 17894, kImageIndexUndefined, }},
        {{ 17895, 17902, }},
        {{ 17896, kImageIndexUndefined, }},
        {{ 17897, kImageIndexUndefined, }},
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
        {{ 17898, kImageIndexUndefined, }},
        {{ 17899, kImageIndexUndefined, }},
        {{ 17900, 17903, }},
        {{ 17901, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ 17904, kImageIndexUndefined, }},
        {{ 17905, 17912, }},
        {{ 17906, kImageIndexUndefined, }},
        {{ 17907, kImageIndexUndefined, }},
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
        {{ 17908, kImageIndexUndefined, }},
        {{ 17909, kImageIndexUndefined, }},
        {{ 17910, 17913, }},
        {{ 17911, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ 17998, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 17999, 18007, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18008, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18001, 18009, }},
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
            {{ 18002, 18010, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18011, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18004, 18012, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18005, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpSprites = {{
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 0,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 1,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 2,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 3,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 4,
        }},
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 5,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 6,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 7,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 8,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 9,
        }},
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 10,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 11,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 12,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 13,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 14,
        }},
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 15,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 16,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 17,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 18,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 19,
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
            { { 6, 0, 48 }, { 20, 32, 3 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
            {},
            { { 2, 2, 50 }, { 28, 28, 0 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
        }},
        {{
            { { 0, 6, 4 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 30, 20, 3 } },
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
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 20,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 21,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 22,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 23,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 24,
        }},
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 25,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 26,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 27,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 28,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 29,
        }},
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 30,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 31,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 32,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 33,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 34,
        }},
        {{
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 35,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 36,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 37,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 38,
            SPR_G2_BM_TRACK_LARGE_CORKSCREW + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 3, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 5, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 6, SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 40, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 10, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 11, SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 41, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 12, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 13, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 17, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 18, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 19, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 0, 2 }, { 32, 32, 0 } }, {} }},
            {{ { { 2, 0, 0 }, { 1, 32, 160 } }, {} }},
            {{ { { 0, 2, 48 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 0 } }, { { 31, 0, 0 }, { 32, 0, 64 } } }},
            {{ { { 0, 29, 0 }, { 32, 1, 96 } }, {} }},
            {{ { { 0, 0, 140 }, { 32, 32, 3 } }, {} }},
            {{ { { 2, 0, 48 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 32, 0 }, { 32, 0, 64 } } }},
            {{ { { 31, 0, 0 }, { 1, 32, 96 } }, {} }},
            {{ { { 29, 0, 0 }, { 1, 32, 160 } }, {} }},
            {{ { { 0, 0, 48 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 1, 160 } }, {} }},
            {{ { { 0, 0, 48 }, { 32, 32, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 20, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 21, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 22, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 23, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 26, SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 42, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 27, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 28, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 29, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 30, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 31, SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 43, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 33, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 35, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 36, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 37, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 38, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_MEDIUM_HALF_LOOP + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 0, SPR_G2_BM_TRACK_ZERO_G_ROLL + 1, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 2, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 4, SPR_G2_BM_TRACK_ZERO_G_ROLL + 5, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 9, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 10, SPR_G2_BM_TRACK_ZERO_G_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 14, SPR_G2_BM_TRACK_ZERO_G_ROLL + 15, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 32 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 32 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 32 }, { 40, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 31, 0, 0 }, { 1, 40, 32 } } }},
            {{ { { 6, 0, 32 }, { 20, 40, 1 } }, {} }},
            {{ { { 6, 0, 32 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 36 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 36 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 17, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 18, SPR_G2_BM_TRACK_ZERO_G_ROLL + 19, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 22, SPR_G2_BM_TRACK_ZERO_G_ROLL + 23, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 24, SPR_G2_BM_TRACK_ZERO_G_ROLL + 25, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 26, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 28, SPR_G2_BM_TRACK_ZERO_G_ROLL + 29, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 0, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 1, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 2, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 5, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 9, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 10, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 11, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 13, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 14, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 15, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 17, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 18, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 19, kImageIndexUndefined, }},
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
            {{ { { 26, 0, 0 }, { 0, 32, 20 } }, {} }},
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
            {{ { { 18, 0, 0 }, { 1, 32, 30 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 22, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 23, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 25, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 26, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 28, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 29, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 30, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 32, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 33, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 35, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 36, SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 37, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 38, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_LARGE_ZERO_G_ROLL + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeZeroGRollUpBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftFlyerLargeHalfLoopUninvertedDownSprites = {{
        {{
            27694,
            27693,
            27692,
            27691,
            27690,
            27689,
            27688,
        }},
        {{
            27687,
            27686,
            27685,
            27684,
            27683,
            27682,
            27681,
        }},
        {{
            27680,
            27679,
            27678,
            27677,
            27676,
            27675,
            27674,
        }},
        {{
            27673,
            27672,
            27671,
            27670,
            27669,
            27668,
            27667,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftFlyerLargeHalfLoopUninvertedDownOffsets = {{
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftFlyerLargeHalfLoopUninvertedDownBoundBoxes = {{
        {{
            { { 0, 0, 32 }, { 32, 24, 0 } },
            { { 30, 7, 140 }, { 2, 16, 3 } },
            { { 16, 7, 90 }, { 16, 16, 3 } },
            { { 0, 16, 100 }, { 32, 16, 3 } },
            { { 0, 16, 70 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 0, 32 }, { 24, 32, 0 } },
            { { 0, 0, 0 }, { 16, 32, 0 } },
            { { -7, 0, 0 }, { 16, 16, 0 } },
            { { 16, 0, 0 }, { 16, 32, 0 } },
            { { 16, 0, 0 }, { 16, 32, 0 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 16, 32 }, { 32, 24, 0 } },
            { { 0, 16, 0 }, { 32, 16, 0 } },
            { { -7, 16, 0 }, { 16, 16, 0 } },
            { { 0, 0, 0 }, { 32, 16, 0 } },
            { { 0, 0, 0 }, { 32, 16, 0 } },
            { { 0, 6, 29 }, { 32, 20, 19 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
        {{
            { { 16, 0, 32 }, { 24, 32, 0 } },
            { { 16, 30, 140 }, { 16, 2, 3 } },
            { { 23, 30, 110 }, { 16, 2, 3 } },
            { { 0, 0, 100 }, { 16, 32, 3 } },
            { { 0, 0, 70 }, { 16, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightFlyerLargeHalfLoopUninvertedDownSprites = {{
        {{
            27645,
            27644,
            27643,
            27642,
            27641,
            27640,
            27639,
        }},
        {{
            27652,
            27651,
            27650,
            27649,
            27648,
            27647,
            27646,
        }},
        {{
            27659,
            27658,
            27657,
            27656,
            27655,
            27654,
            27653,
        }},
        {{
            27666,
            27665,
            27664,
            27663,
            27662,
            27661,
            27660,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightFlyerLargeHalfLoopUninvertedDownOffsets = {{
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
        {{
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
            { 0, 0, -4 },
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightFlyerLargeHalfLoopUninvertedDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftFlyerLargeHalfLoopUninvertedDownBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlyerHalfLoopUninvertedDownSprites = {{
        {{
            27454,
            27453,
            27452,
            27451,
        }},
        {{
            27462,
            27461,
            27460,
            27459,
        }},
        {{
            27455,
            27456,
            27457,
            27458,
        }},
        {{
            27463,
            27464,
            27465,
            27466,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 0,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 1,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 4,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 5,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 8,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 9,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 12,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 13,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 15,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 16,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 17,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 20,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 21,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 24,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 25,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 28,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 29,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 31,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 32,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 33,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 34,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 35,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 36,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 38,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 39,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 40,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 41,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 42,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 43,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 44,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 45,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 46,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 47,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 48,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 49,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 50,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 51,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 52,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 53,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 54,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 55,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 56,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 57,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 58,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 59,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 60,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 61,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 62,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE + 63,
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 0, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 4, kImageIndexUndefined, }},
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 7, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 9, kImageIndexUndefined, }},
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 10, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 14, kImageIndexUndefined, }},
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 17, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 19, kImageIndexUndefined, }},
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 21,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 23,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 24,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 25,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 27,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 29,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 31,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 32,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 33,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 34,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 35,
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 44, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 46, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 47, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 48, kImageIndexUndefined, }},
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 49, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 50, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 51, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 52, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 53, kImageIndexUndefined, }},
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 54, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 55, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 56, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 57, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 58, kImageIndexUndefined, }},
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
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 59, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 60, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 61, SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 62, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 63, kImageIndexUndefined, }},
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 36,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 37,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 38,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 39,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 40,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 41,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 42,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 43,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 64,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 65,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 66,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 67,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 68,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 69,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 70,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 71,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 72,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 73,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 74,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 75,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 76,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 77,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 78,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 79,
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 80,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 81,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 82,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 83,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 84,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 85,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 86,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 87,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 88,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 89,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 90,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 91,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 92,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 93,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 94,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 95,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 96,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 97,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 98,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 99,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 100,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 101,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 102,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 103,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 104,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 105,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 106,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 107,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 108,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 109,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 110,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 111,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 0, 40 }, { 32, 32, 1 } },
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
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 112,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 113,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 114,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 115,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 116,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 117,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 118,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 119,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 120,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 121,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 122,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 123,
        }},
        {{
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 124,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 125,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 126,
            SPR_G2_BM_TRACK_GENTLE_LARGE_CURVE_BANKED + 127,
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
            SPR_G2_BM_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_DIAG_BRAKES + 1,
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
                SPR_G2_BM_DIAG_BRAKES + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_BM_DIAG_BRAKES + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_BM_DIAG_BRAKES + 3,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_BM_DIAG_BRAKES + 5,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_BM_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_BM_DIAG_BRAKES + 4,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_BM_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_BM_DIAG_BRAKES + 4,
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
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 3,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 5,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 7,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 9,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 11,
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
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 13,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 15,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 18,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 19,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 20,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 21,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 22,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_FLAT_TO_STEEP_DIAGONAL + 23,
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
            SPR_G2_BM_TRACK_DIVE_LOOP + 0,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 1,
            SPR_G2_BM_TRACK_DIVE_LOOP + 2,
            SPR_G2_BM_TRACK_DIVE_LOOP + 3,
        }},
        {{
            SPR_G2_BM_TRACK_DIVE_LOOP + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 5,
            SPR_G2_BM_TRACK_DIVE_LOOP + 6,
            SPR_G2_BM_TRACK_DIVE_LOOP + 7,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 8,
            SPR_G2_BM_TRACK_DIVE_LOOP + 9,
            SPR_G2_BM_TRACK_DIVE_LOOP + 10,
            SPR_G2_BM_TRACK_DIVE_LOOP + 11,
        }},
        {{
            SPR_G2_BM_TRACK_DIVE_LOOP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 13,
            SPR_G2_BM_TRACK_DIVE_LOOP + 14,
            SPR_G2_BM_TRACK_DIVE_LOOP + 15,
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
            { { 0, 6, 32 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, 48 }, { 32, 32, 1 } },
            {},
            {},
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 6, 0, 32 }, { 20, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            { { 2, 2, 4 }, { 28, 28, 1 } },
            { { 0, 0, 0 }, { 32, 1, 64 } },
            { { 0, 6, 32 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, -8 }, { 32, 32, 1 } },
            {},
            {},
            { { 2, 2, 12 }, { 28, 28, 1 } },
            { { 6, 0, 58 }, { 20, 32, 1 } },
            { { 6, 0, 32 }, { 20, 32, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kRightEighthDiveLoopUpToOrthogonalSprites = {{
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 16,
            SPR_G2_BM_TRACK_DIVE_LOOP + 17,
            SPR_G2_BM_TRACK_DIVE_LOOP + 18,
            SPR_G2_BM_TRACK_DIVE_LOOP + 19,
        }},
        {{
            SPR_G2_BM_TRACK_DIVE_LOOP + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 21,
            SPR_G2_BM_TRACK_DIVE_LOOP + 22,
            SPR_G2_BM_TRACK_DIVE_LOOP + 23,
        }},
        {{
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 24,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 25,
            SPR_G2_BM_TRACK_DIVE_LOOP + 26,
            SPR_G2_BM_TRACK_DIVE_LOOP + 27,
        }},
        {{
            SPR_G2_BM_TRACK_DIVE_LOOP + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_TRACK_DIVE_LOOP + 29,
            SPR_G2_BM_TRACK_DIVE_LOOP + 30,
            SPR_G2_BM_TRACK_DIVE_LOOP + 31,
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
        { &kBrakeForDropSprites[0][0], nullptr, &kBrakeForDropBoundBoxes[0][0], false }, // BrakeForDrop
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
        { &kPoweredLiftSprites[0], nullptr, &kPoweredLiftBoundBoxes[0], false }, // PoweredLift
        { &kLeftLargeHalfLoopUpSprites[0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftLargeHalfLoopUp
        { &kRightLargeHalfLoopUpSprites[0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightLargeHalfLoopDown
        { &kLeftFlyerTwistUpSprites[0][0], &kLeftFlyerTwistUpOffsets[0][0], &kLeftFlyerTwistUpBoundBoxes[0][0], false }, // LeftFlyerTwistUp
        { &kRightFlyerTwistUpSprites[0][0], &kRightFlyerTwistUpOffsets[0][0], &kRightFlyerTwistUpBoundBoxes[0][0], false }, // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, true }, // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, true }, // RightFlyerTwistDown
        { &kFlyerHalfLoopUninvertedUpSprites[0][0], &kFlyerHalfLoopUninvertedUpOffsets[0][0], &kFlyerHalfLoopUninvertedUpBoundBoxes[0][0], false }, // FlyerHalfLoopUninvertedUp
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
        { &kUp90ToInvertedFlatQuarterLoopSprites[0][0], nullptr, &kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &kInvertedFlatToDown90QuarterLoopSprites[0][0], nullptr, &kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0], false }, // InvertedFlatToDown90QuarterLoop
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
        { &kUp90ToInvertedFlatQuarterLoopSprites[0][0], nullptr, &kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0], false }, // MultiDimUp90ToInvertedFlatQuarterLoop
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
        { &kLeftLargeHalfLoopUpSprites[0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftFlyerLargeHalfLoopUninvertedUp
        { &kRightLargeHalfLoopUpSprites[0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // LeftFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false }, // RightFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerLargeHalfLoopInvertedUp
        { &kLeftFlyerLargeHalfLoopUninvertedDownSprites[0][0], &kLeftFlyerLargeHalfLoopUninvertedDownOffsets[0][0], &kLeftFlyerLargeHalfLoopUninvertedDownBoundBoxes[0][0], false }, // LeftFlyerLargeHalfLoopUninvertedDown
        { &kRightFlyerLargeHalfLoopUninvertedDownSprites[0][0], &kRightFlyerLargeHalfLoopUninvertedDownOffsets[0][0], &kRightFlyerLargeHalfLoopUninvertedDownBoundBoxes[0][0], false }, // RightFlyerLargeHalfLoopUninvertedDown
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
} // namespace OpenRCT2::TwisterRollerCoaster
