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

namespace OpenRCT2::MultiDimensionRollerCoasterInverted
{
    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatSprites = {{
        26227,
        26228,
        26227,
        26228,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 22 }, { 32, 20, 1 } },
        { { 6, 0, 22 }, { 20, 32, 1 } },
        { { 0, 6, 22 }, { 32, 20, 1 } },
        { { 6, 0, 22 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25Sprites = {{
        26253,
        26254,
        26255,
        26256,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 6, 40 }, { 32, 20, 3 } },
        { { 6, 0, 40 }, { 20, 32, 3 } },
        { { 0, 6, 40 }, { 32, 20, 3 } },
        { { 6, 0, 40 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp60Sprites = {{
        26269,
        26270,
        26271,
        26272,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp60Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        { { 0, 6, 88 }, { 32, 20, 3 } },
        { { 4, 0, 6 }, { 2, 32, 81 } },
        { { 0, 4, 6 }, { 32, 2, 81 } },
        { { 6, 0, 88 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        26245,
        26246,
        26247,
        26248,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToUp25Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ 26257, kImageIndexUndefined, }},
        {{ 26261, 26258, }},
        {{ 26262, 26259, }},
        {{ 26260, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Offsets = {{
        {{ { 0, 0, 24 }, {} }},
        {{ { 0, 0, 24 }, { 0, 0, 24 } }},
        {{ { 0, 0, 24 }, { 0, 0, 24 } }},
        {{ { 0, 0, 24 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 56 }, { 32, 20, 3 } }, {} }},
        {{ { { 10, 0, 6 }, { 10, 32, 49 } }, { { 4, 0, 6 }, { 2, 32, 49 } } }},
        {{ { { 0, 10, 6 }, { 32, 10, 49 } }, { { 0, 4, 6 }, { 32, 2, 49 } } }},
        {{ { { 6, 0, 56 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Sprites = {{
        {{ 26263, kImageIndexUndefined, }},
        {{ 26267, 26264, }},
        {{ 26268, 26265, }},
        {{ 26266, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Offsets = {{
        {{ { 0, 0, 24 }, {} }},
        {{ { 0, 0, 24 }, { 0, 0, 24 } }},
        {{ { 0, 0, 24 }, { 0, 0, 24 } }},
        {{ { 0, 0, 24 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 56 }, { 32, 20, 3 } }, {} }},
        {{ { { 10, 0, 6 }, { 10, 32, 49 } }, { { 4, 0, 6 }, { 2, 32, 49 } } }},
        {{ { { 0, 10, 6 }, { 32, 10, 49 } }, { { 0, 4, 6 }, { 32, 2, 49 } } }},
        {{ { { 6, 0, 56 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        26249,
        26250,
        26251,
        26252,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToFlatOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            26310,
            kImageIndexUndefined,
            26309,
            26308,
            kImageIndexUndefined,
            26307,
            26306,
        }},
        {{
            26315,
            kImageIndexUndefined,
            26314,
            26313,
            kImageIndexUndefined,
            26312,
            26311,
        }},
        {{
            26320,
            kImageIndexUndefined,
            26319,
            26318,
            kImageIndexUndefined,
            26317,
            26316,
        }},
        {{
            26305,
            kImageIndexUndefined,
            26304,
            26303,
            kImageIndexUndefined,
            26302,
            26301,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 22 }, { 32, 16, 3 } },
            { { 0, 16, 22 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 22 }, { 16, 32, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 22 }, { 16, 32, 3 } },
            { { 16, 16, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 22 }, { 32, 16, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 22 }, { 32, 16, 3 } },
            { { 16, 0, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 22 }, { 16, 32, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 22 }, { 16, 32, 3 } },
            { { 0, 0, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 22 }, { 32, 16, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        26273,
        26274,
        26275,
        26276,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        { { 0, 6, 22 }, { 32, 20, 3 } },
        { { 6, 0, 22 }, { 20, 32, 3 } },
        { { 0, 6, 22 }, { 32, 20, 3 } },
        { { 6, 0, 22 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        26277,
        26278,
        26279,
        26280,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToRightBankOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            26354,
            kImageIndexUndefined,
            26353,
            26352,
            kImageIndexUndefined,
            26351,
            26350,
        }},
        {{
            26359,
            kImageIndexUndefined,
            26358,
            26357,
            kImageIndexUndefined,
            26356,
            26355,
        }},
        {{
            26364,
            kImageIndexUndefined,
            26363,
            26362,
            kImageIndexUndefined,
            26361,
            26360,
        }},
        {{
            26349,
            kImageIndexUndefined,
            26348,
            26347,
            kImageIndexUndefined,
            26346,
            26345,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 24 }, { 32, 16, 3 } },
            { { 0, 16, 24 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 24 }, { 16, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 24 }, { 16, 32, 3 } },
            { { 16, 16, 24 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 24 }, { 32, 16, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 24 }, { 32, 16, 3 } },
            { { 16, 0, 24 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 24 }, { 16, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 24 }, { 16, 32, 3 } },
            { { 0, 0, 24 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 24 }, { 32, 16, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        26281,
        26282,
        26283,
        26284,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        26285,
        26286,
        26287,
        26288,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankToUp25Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        26289,
        26290,
        26291,
        26292,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        26293,
        26294,
        26295,
        26296,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        26297,
        26298,
        26299,
        26300,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        { { 0, 6, 22 }, { 32, 20, 3 } },
        { { 6, 0, 22 }, { 20, 32, 3 } },
        { { 0, 6, 22 }, { 32, 20, 3 } },
        { { 6, 0, 22 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            26229,
            26230,
            26231,
            26232,
        }},
        {{
            26233,
            26234,
            26235,
            26236,
        }},
        {{
            26232,
            26231,
            26230,
            26229,
        }},
        {{
            26236,
            26235,
            26234,
            26233,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftOffsets = {{
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            { { 0, 0, 22 }, { 32, 26, 3 } },
            { { 0, 6, 22 }, { 32, 26, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            { { 0, 0, 22 }, { 26, 32, 3 } },
            { { 6, 0, 22 }, { 26, 32, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            { { 0, 6, 22 }, { 32, 26, 3 } },
            { { 0, 0, 22 }, { 32, 26, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            { { 6, 0, 22 }, { 26, 32, 3 } },
            { { 0, 0, 22 }, { 26, 32, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            26237,
            26238,
            26239,
            26240,
        }},
        {{
            26241,
            26242,
            26243,
            26244,
        }},
        {{
            26240,
            26239,
            26238,
            26237,
        }},
        {{
            26244,
            26243,
            26242,
            26241,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightOffsets = {{
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            26326,
            kImageIndexUndefined,
            26325,
            26324,
        }},
        {{
            26329,
            kImageIndexUndefined,
            26328,
            26327,
        }},
        {{
            26332,
            kImageIndexUndefined,
            26331,
            26330,
        }},
        {{
            26323,
            kImageIndexUndefined,
            26322,
            26321,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesOffsets = {{
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 22 }, { 16, 16, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 22 }, { 16, 16, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 22 }, { 16, 16, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 22 }, { 16, 16, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            26338,
            kImageIndexUndefined,
            26337,
            26336,
        }},
        {{
            26341,
            kImageIndexUndefined,
            26340,
            26339,
        }},
        {{
            26344,
            kImageIndexUndefined,
            26343,
            26342,
        }},
        {{
            26335,
            kImageIndexUndefined,
            26334,
            26333,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesOffsets = {{
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 22 }, { 16, 16, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 22 }, { 16, 16, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 22 }, { 16, 16, 3 } },
            { { 6, 0, 22 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 22 }, { 16, 16, 3 } },
            { { 0, 6, 22 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        26549,
        26550,
        26549,
        26550,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBrakesOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 24 }, { 32, 20, 3 } },
        { { 6, 0, 24 }, { 20, 32, 3 } },
        { { 0, 6, 24 }, { 32, 20, 3 } },
        { { 6, 0, 24 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp90Sprites = {{
        {{
            26533,
            kImageIndexUndefined,
        }},
        {{
            26534,
            kImageIndexUndefined,
        }},
        {{
            26535,
            kImageIndexUndefined,
        }},
        {{
            26536,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp90Offsets = {{
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp90BoundBoxes = {{
        {{
            { { 0, 6, 56 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 4, 0, 6 }, { 2, 32, 31 } },
            {},
        }},
        {{
            { { 0, 4, 6 }, { 32, 2, 31 } },
            {},
        }},
        {{
            { { 6, 0, 56 }, { 20, 32, 3 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp90Sprites = {{
        {{
            26525,
            kImageIndexUndefined,
        }},
        {{
            26526,
            kImageIndexUndefined,
        }},
        {{
            26527,
            kImageIndexUndefined,
        }},
        {{
            26528,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp90Offsets = {{
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp90BoundBoxes = {{
        {{
            { { 0, 6, 80 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 4, 0, 6 }, { 2, 32, 55 } },
            {},
        }},
        {{
            { { 0, 4, 6 }, { 32, 2, 55 } },
            {},
        }},
        {{
            { { 6, 0, 80 }, { 20, 32, 3 } },
            {},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kDown90ToDown60Sprites = {{
        26527,
        26528,
        26525,
        26526,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kDown90ToDown60Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kDown90ToDown60BoundBoxes = {{
        { { 0, 4, 6 }, { 32, 2, 55 } },
        { { 6, 0, 80 }, { 20, 32, 3 } },
        { { 0, 6, 80 }, { 32, 20, 3 } },
        { { 4, 0, 6 }, { 2, 32, 55 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp90ToUp60Sprites = {{
        26529,
        26530,
        26531,
        26532,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp90ToUp60Offsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp90ToUp60BoundBoxes = {{
        { { 0, 6, 80 }, { 32, 20, 3 } },
        { { 4, 0, 6 }, { 2, 32, 55 } },
        { { 0, 4, 6 }, { 32, 2, 55 } },
        { { 6, 0, 80 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Sprites = {{
        {{
            26531,
            kImageIndexUndefined,
        }},
        {{
            26532,
            kImageIndexUndefined,
        }},
        {{
            26529,
            kImageIndexUndefined,
        }},
        {{
            26530,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Offsets = {{
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
        {{
            { 0, 0, 24 }, 
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90BoundBoxes = {{
        {{
            { { 0, 4, 6 }, { 32, 2, 55 } },
            {},
        }},
        {{
            { { 6, 0, 80 }, { 20, 32, 3 } },
            {},
        }},
        {{
            { { 0, 6, 80 }, { 32, 20, 3 } },
            {},
        }},
        {{
            { { 4, 0, 6 }, { 2, 32, 55 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            26405,
            26406,
            26407,
            kImageIndexUndefined,
            26408,
        }},
        {{
            26409,
            26410,
            26411,
            kImageIndexUndefined,
            26412,
        }},
        {{
            26413,
            26414,
            26415,
            kImageIndexUndefined,
            26416,
        }},
        {{
            26417,
            26418,
            26419,
            kImageIndexUndefined,
            26420,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagOffsets = {{
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            { { 0, 0, 22 }, { 32, 16, 3 } },
            { { 0, 16, 22 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 22 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            { { 0, 0, 22 }, { 16, 34, 3 } },
            { { 16, 16, 22 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 22 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            { { 0, 16, 22 }, { 32, 16, 3 } },
            { { 16, 0, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 22 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            { { 16, 0, 22 }, { 16, 32, 3 } },
            { { 0, 0, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 22 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            26389,
            26390,
            26391,
            kImageIndexUndefined,
            26392,
        }},
        {{
            26393,
            26394,
            26395,
            kImageIndexUndefined,
            26396,
        }},
        {{
            26397,
            26398,
            26399,
            kImageIndexUndefined,
            26400,
        }},
        {{
            26401,
            26402,
            26403,
            kImageIndexUndefined,
            26404,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagOffsets = {{
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            26437,
            26438,
            26439,
            kImageIndexUndefined,
            26440,
        }},
        {{
            26441,
            26442,
            26443,
            kImageIndexUndefined,
            26444,
        }},
        {{
            26445,
            26446,
            26447,
            kImageIndexUndefined,
            26448,
        }},
        {{
            26449,
            26450,
            26451,
            kImageIndexUndefined,
            26452,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagOffsets = {{
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagBoundBoxes = {{
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            { { 0, 0, 22 }, { 32, 16, 3 } },
            { { 0, 16, 22 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 22 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            { { 0, 0, 22 }, { 16, 34, 3 } },
            { { 16, 16, 22 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 22 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 22 }, { 32, 20, 3 } },
            { { 0, 16, 22 }, { 32, 16, 3 } },
            { { 16, 0, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 22 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 22 }, { 20, 32, 3 } },
            { { 16, 0, 22 }, { 16, 32, 3 } },
            { { 0, 0, 22 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 22 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            26421,
            26422,
            26423,
            kImageIndexUndefined,
            26424,
        }},
        {{
            26425,
            26426,
            26427,
            kImageIndexUndefined,
            26428,
        }},
        {{
            26429,
            26430,
            26431,
            kImageIndexUndefined,
            26432,
        }},
        {{
            26433,
            26434,
            26435,
            kImageIndexUndefined,
            26436,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagOffsets = {{
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
        {{
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            { 0, 0, 24 }, 
            {},
            { 0, 0, 24 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                26453,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26454,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26455,
                kImageIndexUndefined,
            }},
            {{
                26456,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26481,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26482,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26483,
                kImageIndexUndefined,
            }},
            {{
                26484,
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
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 22 }, { 32, 32, 3 } },
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
                26465,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26466,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26467,
                kImageIndexUndefined,
            }},
            {{
                26468,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26493,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26494,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26495,
                kImageIndexUndefined,
            }},
            {{
                26496,
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
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 40 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 40 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 40 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 40 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            kImageIndexUndefined,
            26477,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26478,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26479,
            kImageIndexUndefined,
        }},
        {{
            26480,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60Offsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60BoundBoxes = {{
        {{
            {},
            { { -16, -16, 88 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 24 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 88 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 88 }, { 32, 32, 3 } },
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
                26457,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26458,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26459,
                kImageIndexUndefined,
            }},
            {{
                26460,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26485,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26486,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26487,
                kImageIndexUndefined,
            }},
            {{
                26488,
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
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            kImageIndexUndefined,
            26469,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26470,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26471,
            kImageIndexUndefined,
        }},
        {{
            26472,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Offsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60BoundBoxes = {{
        {{
            {},
            { { -16, -16, 56 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 56 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 56 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 56 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            26473,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26474,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26475,
            kImageIndexUndefined,
        }},
        {{
            26476,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Offsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 56 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { 0, 0, 56 }, { 16, 16, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 56 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 56 }, { 32, 32, 3 } },
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
                26461,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26462,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26463,
                kImageIndexUndefined,
            }},
            {{
                26464,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26489,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26490,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26491,
                kImageIndexUndefined,
            }},
            {{
                26492,
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
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatBoundBoxes = {{
        {{
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            26501,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26502,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26503,
            kImageIndexUndefined,
        }},
        {{
            26504,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankOffsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            kImageIndexUndefined,
            26505,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26506,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26507,
            kImageIndexUndefined,
        }},
        {{
            26508,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankOffsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            26517,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26518,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26519,
            kImageIndexUndefined,
        }},
        {{
            26520,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Offsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            26521,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26522,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26523,
            kImageIndexUndefined,
        }},
        {{
            26524,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Offsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            26509,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26510,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26511,
            kImageIndexUndefined,
        }},
        {{
            26512,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankOffsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            kImageIndexUndefined,
            26513,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26514,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26515,
            kImageIndexUndefined,
        }},
        {{
            26516,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankOffsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 32 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            26497,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26498,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26499,
            kImageIndexUndefined,
        }},
        {{
            26500,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankOffsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankBoundBoxes = {{
        {{
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 22 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistDownSprites = {{
        {{
            26365,
            26366,
            26367,
        }},
        {{
            26371,
            26372,
            26373,
        }},
        {{
            26370,
            26369,
            26368,
        }},
        {{
            26376,
            26375,
            26374,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistDownOffsets = {{
        {{
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftFlyerTwistDownBoundBoxes = {{
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistDownSprites = {{
        {{
            26377,
            26378,
            26379,
        }},
        {{
            26383,
            26384,
            26385,
        }},
        {{
            26382,
            26381,
            26380,
        }},
        {{
            26388,
            26387,
            26386,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistDownOffsets = {{
        {{
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 24 }, 
            { 0, 6, 24 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 24 }, 
            { 6, 0, 24 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightFlyerTwistDownBoundBoxes = {{
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 24 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 24 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kMultiDimInvertedFlatToDown90QuarterLoopSprites = {{
        {{
            16208,
            16207,
            16206,
            kImageIndexUndefined,
        }},
        {{
            16211,
            16210,
            16209,
            kImageIndexUndefined,
        }},
        {{
            16214,
            16213,
            16212,
            kImageIndexUndefined,
        }},
        {{
            16217,
            16216,
            16215,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kMultiDimInvertedFlatToDown90QuarterLoopBoundBoxes = {{
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
            26551,
            26552,
            26551,
            26552,
        }},
        {{
            26553,
            26554,
            26553,
            26554,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBlockBrakesOffsets = {{
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
        { 0, 0, 24 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 24 }, { 32, 20, 3 } },
        { { 6, 0, 24 }, { 20, 32, 3 } },
        { { 0, 6, 24 }, { 32, 20, 3 } },
        { { 6, 0, 24 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kMultiDimInvertedUp90ToFlatQuarterLoopSprites = {{
        {{
            26537,
            26538,
            26539,
        }},
        {{
            26540,
            26541,
            26542,
        }},
        {{
            26543,
            26544,
            26545,
        }},
        {{
            26546,
            26547,
            26548,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 3>, kNumOrthogonalDirections>
    kMultiDimInvertedUp90ToFlatQuarterLoopOffsets = {{
        {{
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
        }},
        {{
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
        }},
        {{
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
        }},
        {{
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
            { 0, 0, -16 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kMultiDimInvertedUp90ToFlatQuarterLoopBoundBoxes = {{
        {{
            { { 28, 6, -8 }, { 2, 20, 31 } },
            { { 24, 6, -16 }, { 2, 20, 31 } },
            { { 0, 6, -16 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, -8 }, { 20, 2, 31 } },
            { { 6, 0, -8 }, { 20, 2, 63 } },
            { { 6, 0, -8 }, { 20, 2, 31 } },
        }},
        {{
            { { 0, 6, -8 }, { 2, 20, 31 } },
            { { 0, 6, -8 }, { 2, 20, 63 } },
            { { 0, 6, -8 }, { 2, 20, 31 } },
        }},
        {{
            { { 6, 28, -8 }, { 20, 2, 31 } },
            { { 6, 24, -16 }, { 20, 2, 31 } },
            { { 6, 0, -16 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesOffsets = {{
        {{
            {},
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 24 }, { 32, 32, -3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 24 }, { 32, 32, -3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 24 }, { 32, 32, -3 } },
            {},
        }},
        {{
            { { -16, -16, 24 }, { 32, 32, -3 } },
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
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 4,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 3,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_MULTIDIM_INVERTED_DIAG_BRAKES + 3,
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
            { -16, -16, 24 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 24 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 24 }, 
            {},
        }},
        {{
            { -16, -16, 24 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 24 }, { 32, 32, -3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 24 }, { 32, 32, -3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 24 }, { 32, 32, -3 } },
            {},
        }},
        {{
            { { -16, -16, 24 }, { 32, 32, -3 } },
            {},
            {},
            {},
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { nullptr, nullptr, nullptr, false }, // EndStation
        { nullptr, nullptr, nullptr, false }, // BeginStation
        { nullptr, nullptr, nullptr, false }, // MiddleStation
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
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], &kSBendLeftOffsets[0][0], &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], &kSBendRightOffsets[0][0], &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], &kLeftQuarterTurn3TilesOffsets[0][0], &kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0], &kLeftBankedQuarterTurn3TilesOffsets[0][0], &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftBankedQuarterTurn3Tiles
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
        { nullptr, nullptr, nullptr, false }, // Up25LeftBanked
        { nullptr, nullptr, nullptr, false }, // Up25RightBanked
        { nullptr, nullptr, nullptr, false }, // Waterfall
        { nullptr, nullptr, nullptr, false }, // Rapids
        { &kFlatSprites[0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // OnRidePhoto
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
        { &kLeftFlyerTwistDownSprites[0][0], &kLeftFlyerTwistDownOffsets[0][0], &kLeftFlyerTwistDownBoundBoxes[0][0], false }, // LeftFlyerTwistDown
        { &kRightFlyerTwistDownSprites[0][0], &kRightFlyerTwistDownOffsets[0][0], &kRightFlyerTwistDownBoundBoxes[0][0], false }, // RightFlyerTwistDown
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
        { &kMultiDimInvertedFlatToDown90QuarterLoopSprites[0][0], nullptr, &kMultiDimInvertedFlatToDown90QuarterLoopBoundBoxes[0][0], false }, // MultiDimInvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // Up90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr, false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0], &kBlockBrakesOffsets[0], &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
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
        { &kMultiDimInvertedUp90ToFlatQuarterLoopSprites[0][0], &kMultiDimInvertedUp90ToFlatQuarterLoopOffsets[0][0], &kMultiDimInvertedUp90ToFlatQuarterLoopBoundBoxes[0][0], false }, // MultiDimInvertedUp90ToFlatQuarterLoop
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
} // namespace OpenRCT2::MultiDimensionRollerCoasterInverted
