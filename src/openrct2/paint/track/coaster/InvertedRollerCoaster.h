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

namespace OpenRCT2::InvertedRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            27129,
            27130,
            27129,
            27130,
        }},
        {{
            27131,
            27132,
            27131,
            27132,
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
            {{ 27131, SPR_STATION_INVERTED_BAR_C_SW_NE, }},
            {{ 27132, SPR_STATION_INVERTED_BAR_C_NW_SE, }},
            {{ 27131, SPR_STATION_INVERTED_BAR_C_SW_NE, }},
            {{ 27132, SPR_STATION_INVERTED_BAR_C_NW_SE, }},
        }},
        {{
            {{ 27131, kImageIndexUndefined, }},
            {{ 27132, kImageIndexUndefined, }},
            {{ 27131, kImageIndexUndefined, }},
            {{ 27132, kImageIndexUndefined, }},
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
            27221,
            27222,
            27223,
            27224,
        }},
        {{
            27249,
            27250,
            27251,
            27252,
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp60Sprites = {{
        27237,
        27238,
        27239,
        27240,
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
            27213,
            27214,
            27215,
            27216,
        }},
        {{
            27241,
            27242,
            27243,
            27244,
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
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ 27225, kImageIndexUndefined, }},
        {{ 27229, 27226, }},
        {{ 27230, 27227, }},
        {{ 27228, kImageIndexUndefined, }},
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
        {{ 27231, kImageIndexUndefined, }},
        {{ 27235, 27232, }},
        {{ 27236, 27233, }},
        {{ 27234, kImageIndexUndefined, }},
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
            27217,
            27218,
            27219,
            27220,
        }},
        {{
            27245,
            27246,
            27247,
            27248,
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
            27142,
            kImageIndexUndefined,
            27141,
            27140,
            kImageIndexUndefined,
            27139,
            27138,
        }},
        {{
            27147,
            kImageIndexUndefined,
            27146,
            27145,
            kImageIndexUndefined,
            27144,
            27143,
        }},
        {{
            27152,
            kImageIndexUndefined,
            27151,
            27150,
            kImageIndexUndefined,
            27149,
            27148,
        }},
        {{
            27137,
            kImageIndexUndefined,
            27136,
            27135,
            kImageIndexUndefined,
            27134,
            27133,
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
        27269,
        27270,
        27271,
        27272,
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
        27273,
        27274,
        27275,
        27276,
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
            27162,
            kImageIndexUndefined,
            27161,
            27160,
            kImageIndexUndefined,
            27159,
            27158,
        }},
        {{
            27167,
            kImageIndexUndefined,
            27166,
            27165,
            kImageIndexUndefined,
            27164,
            27163,
        }},
        {{
            27172,
            kImageIndexUndefined,
            27171,
            27170,
            kImageIndexUndefined,
            27169,
            27168,
        }},
        {{
            27157,
            kImageIndexUndefined,
            27156,
            27155,
            kImageIndexUndefined,
            27154,
            27153,
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
        27277,
        27278,
        27279,
        27280,
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
        27281,
        27282,
        27283,
        27284,
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
        27285,
        27286,
        27287,
        27288,
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
        27289,
        27290,
        27291,
        27292,
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
        27293,
        27294,
        27295,
        27296,
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
            27317,
            kImageIndexUndefined,
            27318,
            27319,
            kImageIndexUndefined,
            27320,
            27321,
        }},
        {{
            27322,
            kImageIndexUndefined,
            27323,
            27324,
            kImageIndexUndefined,
            27325,
            27326,
        }},
        {{
            27327,
            kImageIndexUndefined,
            27328,
            27329,
            kImageIndexUndefined,
            27330,
            27331,
        }},
        {{
            27332,
            kImageIndexUndefined,
            27333,
            27334,
            kImageIndexUndefined,
            27335,
            27336,
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
            27297,
            kImageIndexUndefined,
            27298,
            27299,
            kImageIndexUndefined,
            27300,
            27301,
        }},
        {{
            27302,
            kImageIndexUndefined,
            27303,
            27304,
            kImageIndexUndefined,
            27305,
            27306,
        }},
        {{
            27307,
            kImageIndexUndefined,
            27308,
            27309,
            kImageIndexUndefined,
            27310,
            27311,
        }},
        {{
            27312,
            kImageIndexUndefined,
            27313,
            27314,
            kImageIndexUndefined,
            27315,
            27316,
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
            27253,
            27254,
            27255,
            27256,
        }},
        {{
            27257,
            27258,
            27259,
            27260,
        }},
        {{
            27256,
            27255,
            27254,
            27253,
        }},
        {{
            27260,
            27259,
            27258,
            27257,
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
            27261,
            27262,
            27263,
            27264,
        }},
        {{
            27265,
            27266,
            27267,
            27268,
        }},
        {{
            27264,
            27263,
            27262,
            27261,
        }},
        {{
            27268,
            27267,
            27266,
            27265,
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
            27355,
            27356,
            27357,
            27358,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27359,
            27360,
            27361,
            27362,
        }},
        {{
            27363,
            27364,
            27365,
            27366,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27367,
            27368,
            27369,
            27370,
        }},
        {{
            27362,
            27361,
            27360,
            27359,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27358,
            27357,
            27356,
            27355,
        }},
        {{
            27370,
            27369,
            27368,
            27367,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27366,
            27365,
            27364,
            27363,
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
            27386,
            27385,
            27384,
            27383,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27382,
            27381,
            27380,
            27379,
        }},
        {{
            27378,
            27377,
            27376,
            27375,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27374,
            27373,
            27372,
            27371,
        }},
        {{
            27379,
            27380,
            27381,
            27382,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27383,
            27384,
            27385,
            27386,
        }},
        {{
            27371,
            27372,
            27373,
            27374,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27375,
            27376,
            27377,
            27378,
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
            27392,
            kImageIndexUndefined,
            27391,
            27390,
        }},
        {{
            27395,
            kImageIndexUndefined,
            27394,
            27393,
        }},
        {{
            27398,
            kImageIndexUndefined,
            27397,
            27396,
        }},
        {{
            27389,
            kImageIndexUndefined,
            27388,
            27387,
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
            27404,
            kImageIndexUndefined,
            27403,
            27402,
        }},
        {{
            27407,
            kImageIndexUndefined,
            27406,
            27405,
        }},
        {{
            27410,
            kImageIndexUndefined,
            27409,
            27408,
        }},
        {{
            27401,
            kImageIndexUndefined,
            27400,
            27399,
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
            27422,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27421,
        }},
        {{
            27424,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27423,
        }},
        {{
            27426,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27425,
        }},
        {{
            27420,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27419,
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
            27411,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27412,
        }},
        {{
            27413,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27414,
        }},
        {{
            27415,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27416,
        }},
        {{
            27417,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27418,
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
            27427,
            27428,
            27429,
        }},
        {{
            27433,
            27434,
            27435,
        }},
        {{
            27432,
            27431,
            27430,
        }},
        {{
            27438,
            27437,
            27436,
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
            27439,
            27440,
            27441,
        }},
        {{
            27445,
            27446,
            27447,
        }},
        {{
            27444,
            27443,
            27442,
        }},
        {{
            27450,
            27449,
            27448,
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
            27451,
            27452,
            27453,
            27454,
        }},
        {{
            27459,
            27460,
            27461,
            27462,
        }},
        {{
            27458,
            27457,
            27456,
            27455,
        }},
        {{
            27466,
            27465,
            27464,
            27463,
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
            27467,
            27468,
            27469,
        }},
        {{
            27470,
            27471,
            27472,
        }},
        {{
            27473,
            27474,
            27475,
        }},
        {{
            27476,
            27477,
            27478,
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
            27479,
            27480,
            27481,
        }},
        {{
            27482,
            27483,
            27484,
        }},
        {{
            27485,
            27486,
            27487,
        }},
        {{
            27488,
            27489,
            27490,
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
        {{ 27352, 27348, }},
        {{ 27353, 27349, }},
        {{ 27354, 27350, }},
        {{ 27351, 27347, }},
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
        {{ 27343, 27339, }},
        {{ 27344, 27340, }},
        {{ 27345, 27341, }},
        {{ 27346, 27342, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        27337,
        27338,
        27337,
        27338,
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBoosterSprites = {{
        SPR_G2_BM_INVERT_BOOSTER_1 + 0,
        SPR_G2_BM_INVERT_BOOSTER_2 + 0,
        SPR_G2_BM_INVERT_BOOSTER_1 + 0,
        SPR_G2_BM_INVERT_BOOSTER_2 + 0,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBoosterOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBoosterBoundBoxes = {{
        { { 0, 6, 29 }, { 32, 20, 1 } },
        { { 6, 0, 29 }, { 20, 32, 1 } },
        { { 0, 6, 29 }, { 32, 20, 1 } },
        { { 6, 0, 29 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterBankedHelixLargeUpSprites = {{
        {{
            27202,
            kImageIndexUndefined,
            27201,
            27200,
            kImageIndexUndefined,
            27199,
            27198,
        }},
        {{
            27207,
            kImageIndexUndefined,
            27206,
            27205,
            kImageIndexUndefined,
            27204,
            27203,
        }},
        {{
            27212,
            kImageIndexUndefined,
            27211,
            27210,
            kImageIndexUndefined,
            27209,
            27208,
        }},
        {{
            27197,
            kImageIndexUndefined,
            27196,
            27195,
            kImageIndexUndefined,
            27194,
            27193,
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
            27173,
            kImageIndexUndefined,
            27174,
            27175,
            kImageIndexUndefined,
            27176,
            27177,
        }},
        {{
            27178,
            kImageIndexUndefined,
            27179,
            27180,
            kImageIndexUndefined,
            27181,
            27182,
        }},
        {{
            27183,
            kImageIndexUndefined,
            27184,
            27185,
            kImageIndexUndefined,
            27186,
            27187,
        }},
        {{
            27188,
            kImageIndexUndefined,
            27189,
            27190,
            kImageIndexUndefined,
            27191,
            27192,
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25LeftBankedSprites = {{
        27711,
        27712,
        27713,
        27714,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25LeftBankedOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25LeftBankedBoundBoxes = {{
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25RightBankedSprites = {{
        27715,
        27716,
        27717,
        27718,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25RightBankedOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            27507,
            27508,
            27509,
            kImageIndexUndefined,
            27510,
        }},
        {{
            27511,
            27512,
            27513,
            kImageIndexUndefined,
            27514,
        }},
        {{
            27515,
            27516,
            27517,
            kImageIndexUndefined,
            27518,
        }},
        {{
            27519,
            27520,
            27521,
            kImageIndexUndefined,
            27522,
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
            27491,
            27492,
            27493,
            kImageIndexUndefined,
            27494,
        }},
        {{
            27495,
            27496,
            27497,
            kImageIndexUndefined,
            27498,
        }},
        {{
            27499,
            27500,
            27501,
            kImageIndexUndefined,
            27502,
        }},
        {{
            27503,
            27504,
            27505,
            kImageIndexUndefined,
            27506,
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
            27539,
            27540,
            27541,
            kImageIndexUndefined,
            27542,
        }},
        {{
            27543,
            27544,
            27545,
            kImageIndexUndefined,
            27546,
        }},
        {{
            27547,
            27548,
            27549,
            kImageIndexUndefined,
            27550,
        }},
        {{
            27551,
            27552,
            27553,
            kImageIndexUndefined,
            27554,
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
            27523,
            27524,
            27525,
            kImageIndexUndefined,
            27526,
        }},
        {{
            27527,
            27528,
            27529,
            kImageIndexUndefined,
            27530,
        }},
        {{
            27531,
            27532,
            27533,
            kImageIndexUndefined,
            27534,
        }},
        {{
            27535,
            27536,
            27537,
            kImageIndexUndefined,
            27538,
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
                27555,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27556,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27557,
                kImageIndexUndefined,
            }},
            {{
                27558,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                27611,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27612,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27613,
                kImageIndexUndefined,
            }},
            {{
                27614,
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
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 1 } },
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
                27567,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27568,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27569,
                kImageIndexUndefined,
            }},
            {{
                27570,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                27623,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27624,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27625,
                kImageIndexUndefined,
            }},
            {{
                27626,
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
            27579,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27580,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27581,
            kImageIndexUndefined,
        }},
        {{
            27582,
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
                27559,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27560,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27561,
                kImageIndexUndefined,
            }},
            {{
                27562,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                27615,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27616,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27617,
                kImageIndexUndefined,
            }},
            {{
                27618,
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
            27571,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27572,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27573,
            kImageIndexUndefined,
        }},
        {{
            27574,
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
            27575,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27576,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27577,
            kImageIndexUndefined,
        }},
        {{
            27578,
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
                27563,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27564,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27565,
                kImageIndexUndefined,
            }},
            {{
                27566,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                27619,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                27620,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                27621,
                kImageIndexUndefined,
            }},
            {{
                27622,
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
            27587,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27588,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27589,
            kImageIndexUndefined,
        }},
        {{
            27590,
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
            27591,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27592,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27593,
            kImageIndexUndefined,
        }},
        {{
            27594,
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
            27603,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27604,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27605,
            kImageIndexUndefined,
        }},
        {{
            27606,
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
            27607,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27608,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27609,
            kImageIndexUndefined,
        }},
        {{
            27610,
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
            27595,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27596,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27597,
            kImageIndexUndefined,
        }},
        {{
            27598,
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
            27599,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27600,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27601,
            kImageIndexUndefined,
        }},
        {{
            27602,
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
            27583,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27584,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            27585,
            kImageIndexUndefined,
        }},
        {{
            27586,
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
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            27639,
            27640,
            27641,
            27642,
            27643,
            27644,
            27645,
        }},
        {{
            27646,
            27647,
            27648,
            27649,
            27650,
            27651,
            27652,
        }},
        {{
            27653,
            27654,
            27655,
            27656,
            27657,
            27658,
            27659,
        }},
        {{
            27660,
            27661,
            27662,
            27663,
            27664,
            27665,
            27666,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpBoundBoxes = {{
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 8, 0, 40 }, { 24, 16, 3 } },
            { { 24, 0, 29 }, { 8, 16, 3 } },
            { { 30, 30, 80 }, { 2, 2, 3 } },
            { { 30, 16, 0 }, { 2, 16, 3 } },
            { { 0, 16, 32 }, { 32, 16, 0 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 9 } },
            { { 0, 0, 0 }, { 16, 32, 0 } },
            { { 0, 0, 0 }, { 16, 32, 0 } },
            { { 16, 0, 0 }, { 16, 16, 0 } },
            { { 16, 0, 0 }, { 16, 32, 0 } },
            { { 16, 0, 0 }, { 16, 32, 0 } },
        }},
        {{
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
            { { 0, 16, 0 }, { 32, 16, 0 } },
            { { 0, 16, 0 }, { 32, 16, 0 } },
            { { 0, 0, 0 }, { 16, 16, 0 } },
            { { 0, 0, 0 }, { 32, 16, 0 } },
            { { 0, 0, 0 }, { 32, 16, 0 } },
        }},
        {{
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 6, 0, 29 }, { 20, 32, 3 } },
            { { 16, 30, 70 }, { 16, 2, 3 } },
            { { 16, 30, 100 }, { 16, 2, 3 } },
            { { 0, 30, 90 }, { 16, 2, 3 } },
            { { 0, 30, 140 }, { 16, 2, 3 } },
            { { 0, 0, 32 }, { 16, 32, 0 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpSprites = {{
        {{
            27688,
            27689,
            27690,
            27691,
            27692,
            27693,
            27694,
        }},
        {{
            27681,
            27682,
            27683,
            27684,
            27685,
            27686,
            27687,
        }},
        {{
            27674,
            27675,
            27676,
            27677,
            27678,
            27679,
            27680,
        }},
        {{
            27667,
            27668,
            27669,
            27670,
            27671,
            27672,
            27673,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBlockBrakesSprites = {{
        27337,
        27338,
        27337,
        27338,
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
    kLeftBankedQuarterTurn3TileUp25Sprites = {{
        {{
            27762,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27761,
        }},
        {{
            27764,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27763,
        }},
        {{
            27766,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27765,
        }},
        {{
            27760,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27759,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Offsets = {{
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
    kLeftBankedQuarterTurn3TileUp25BoundBoxes = {{
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
    kRightBankedQuarterTurn3TileUp25Sprites = {{
        {{
            27751,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27752,
        }},
        {{
            27753,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27754,
        }},
        {{
            27755,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27756,
        }},
        {{
            27757,
            kImageIndexUndefined,
            kImageIndexUndefined,
            27758,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Offsets = {{
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
    kRightBankedQuarterTurn3TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn3TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn5TileUp25Sprites = {{
        {{
            27787,
            kImageIndexUndefined,
            27788,
            27789,
            kImageIndexUndefined,
            27790,
            27791,
        }},
        {{
            27792,
            kImageIndexUndefined,
            27793,
            27794,
            kImageIndexUndefined,
            27795,
            27796,
        }},
        {{
            27797,
            kImageIndexUndefined,
            27798,
            27799,
            kImageIndexUndefined,
            27800,
            27801,
        }},
        {{
            27802,
            kImageIndexUndefined,
            27803,
            27804,
            kImageIndexUndefined,
            27805,
            27806,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn5TileUp25Offsets = {{
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
    kLeftBankedQuarterTurn5TileUp25BoundBoxes = {{
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
    kRightBankedQuarterTurn5TileUp25Sprites = {{
        {{
            27767,
            kImageIndexUndefined,
            27768,
            27769,
            kImageIndexUndefined,
            27770,
            27771,
        }},
        {{
            27772,
            kImageIndexUndefined,
            27773,
            27774,
            kImageIndexUndefined,
            27775,
            27776,
        }},
        {{
            27777,
            kImageIndexUndefined,
            27778,
            27779,
            kImageIndexUndefined,
            27780,
            27781,
        }},
        {{
            27782,
            kImageIndexUndefined,
            27783,
            27784,
            kImageIndexUndefined,
            27785,
            27786,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25Offsets = {{
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
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        27719,
        27720,
        27721,
        27722,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25BoundBoxes = {{
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25Sprites = {{
        27723,
        27724,
        27725,
        27726,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        27727,
        27728,
        27729,
        27730,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25BoundBoxes = {{
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
        { { 0, 6, 45 }, { 32, 20, 3 } },
        { { 6, 0, 45 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25Sprites = {{
        27731,
        27732,
        27733,
        27734,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        27735,
        27736,
        27737,
        27738,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25BoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25Sprites = {{
        27739,
        27740,
        27741,
        27742,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        27743,
        27744,
        27745,
        27746,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatBoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatSprites = {{
        27747,
        27748,
        27749,
        27750,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        27695,
        27696,
        27697,
        27698,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25BoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToRightBankedUp25Sprites = {{
        27699,
        27700,
        27701,
        27702,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToRightBankedUp25Offsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        27703,
        27704,
        27705,
        27706,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatBoundBoxes = {{
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
        { { 0, 6, 37 }, { 32, 20, 3 } },
        { { 6, 0, 37 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatSprites = {{
        27707,
        27708,
        27709,
        27710,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatOffsets = {{
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
        { 0, 0, 29 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_BM_INVERT_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_INVERT_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_INVERT_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_INVERT_DIAG_BRAKES + 1,
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
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_BM_INVERT_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_INVERT_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_BM_INVERT_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_BM_INVERT_DIAG_BRAKES + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 29 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 29 }, { 32, 32, 1 } },
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
        { &kUp60Sprites[0], &kUp60Offsets[0], &kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0], &kUp25ToUp60Offsets[0][0], &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0], &kUp60ToUp25Offsets[0][0], &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
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
        { &kLeftQuarterTurn1TileUp60Sprites[0][0], nullptr, &kLeftQuarterTurn1TileUp60BoundBoxes[0][0], false }, // LeftQuarterTurn1TileUp60
        { &kRightQuarterTurn1TileUp60Sprites[0][0], nullptr, &kRightQuarterTurn1TileUp60BoundBoxes[0][0], false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0], &kBrakesOffsets[0], &kBrakesBoundBoxes[0], false }, // Brakes
        { &kBoosterSprites[0], &kBoosterOffsets[0], &kBoosterBoundBoxes[0], false }, // Booster
        { nullptr, nullptr, nullptr, false }, // Maze
        { &kLeftQuarterBankedHelixLargeUpSprites[0][0], &kLeftQuarterBankedHelixLargeUpOffsets[0][0], &kLeftQuarterBankedHelixLargeUpBoundBoxes[0][0], false }, // LeftQuarterBankedHelixLargeUp
        { &kRightQuarterBankedHelixLargeUpSprites[0][0], &kRightQuarterBankedHelixLargeUpOffsets[0][0], &kRightQuarterBankedHelixLargeUpBoundBoxes[0][0], false }, // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, true }, // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, true }, // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, false }, // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, false }, // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, true }, // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, true }, // RightQuarterHelixLargeDown
        { &kUp25LeftBankedSprites[0], &kUp25LeftBankedOffsets[0], &kUp25LeftBankedBoundBoxes[0], false }, // Up25LeftBanked
        { &kUp25RightBankedSprites[0], &kUp25RightBankedOffsets[0], &kUp25RightBankedBoundBoxes[0], false }, // Up25RightBanked
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
        { nullptr, nullptr, nullptr, false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { nullptr, nullptr, nullptr, false }, // Up60ToUp90
        { nullptr, nullptr, nullptr, false }, // Down90ToDown60
        { nullptr, nullptr, nullptr, false }, // Up90ToUp60
        { nullptr, nullptr, nullptr, false }, // Down60ToDown90
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
        { &kLeftLargeHalfLoopUpSprites[0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftLargeHalfLoopUp
        { &kRightLargeHalfLoopUpSprites[0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightLargeHalfLoopUp
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
        { &kLeftBankedQuarterTurn3TileUp25Sprites[0][0], &kLeftBankedQuarterTurn3TileUp25Offsets[0][0], &kLeftBankedQuarterTurn3TileUp25BoundBoxes[0][0], false }, // LeftBankedQuarterTurn3TileUp25
        { &kRightBankedQuarterTurn3TileUp25Sprites[0][0], &kRightBankedQuarterTurn3TileUp25Offsets[0][0], &kRightBankedQuarterTurn3TileUp25BoundBoxes[0][0], false }, // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3TileDown25
        { &kLeftBankedQuarterTurn5TileUp25Sprites[0][0], &kLeftBankedQuarterTurn5TileUp25Offsets[0][0], &kLeftBankedQuarterTurn5TileUp25BoundBoxes[0][0], false }, // LeftBankedQuarterTurn5TileUp25
        { &kRightBankedQuarterTurn5TileUp25Sprites[0][0], &kRightBankedQuarterTurn5TileUp25Offsets[0][0], &kRightBankedQuarterTurn5TileUp25BoundBoxes[0][0], false }, // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn5TileDown25
        { &kUp25ToLeftBankedUp25Sprites[0], &kUp25ToLeftBankedUp25Offsets[0], &kUp25ToLeftBankedUp25BoundBoxes[0], false }, // Up25ToLeftBankedUp25
        { &kUp25ToRightBankedUp25Sprites[0], &kUp25ToRightBankedUp25Offsets[0], &kUp25ToRightBankedUp25BoundBoxes[0], false }, // Up25ToRightBankedUp25
        { &kLeftBankedUp25ToUp25Sprites[0], &kLeftBankedUp25ToUp25Offsets[0], &kLeftBankedUp25ToUp25BoundBoxes[0], false }, // LeftBankedUp25ToUp25
        { &kRightBankedUp25ToUp25Sprites[0], &kRightBankedUp25ToUp25Offsets[0], &kRightBankedUp25ToUp25BoundBoxes[0], false }, // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToDown25
        { &kLeftBankedFlatToLeftBankedUp25Sprites[0], &kLeftBankedFlatToLeftBankedUp25Offsets[0], &kLeftBankedFlatToLeftBankedUp25BoundBoxes[0], false }, // LeftBankedFlatToLeftBankedUp25
        { &kRightBankedFlatToRightBankedUp25Sprites[0], &kRightBankedFlatToRightBankedUp25Offsets[0], &kRightBankedFlatToRightBankedUp25BoundBoxes[0], false }, // RightBankedFlatToRightBankedUp25
        { &kLeftBankedUp25ToLeftBankedFlatSprites[0], &kLeftBankedUp25ToLeftBankedFlatOffsets[0], &kLeftBankedUp25ToLeftBankedFlatBoundBoxes[0], false }, // LeftBankedUp25ToLeftBankedFlat
        { &kRightBankedUp25ToRightBankedFlatSprites[0], &kRightBankedUp25ToRightBankedFlatOffsets[0], &kRightBankedUp25ToRightBankedFlatBoundBoxes[0], false }, // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToRightBankedFlat
        { &kFlatToLeftBankedUp25Sprites[0], &kFlatToLeftBankedUp25Offsets[0], &kFlatToLeftBankedUp25BoundBoxes[0], false }, // FlatToLeftBankedUp25
        { &kFlatToRightBankedUp25Sprites[0], &kFlatToRightBankedUp25Offsets[0], &kFlatToRightBankedUp25BoundBoxes[0], false }, // FlatToRightBankedUp25
        { &kLeftBankedUp25ToFlatSprites[0], &kLeftBankedUp25ToFlatOffsets[0], &kLeftBankedUp25ToFlatBoundBoxes[0], false }, // LeftBankedUp25ToFlat
        { &kRightBankedUp25ToFlatSprites[0], &kRightBankedUp25ToFlatOffsets[0], &kRightBankedUp25ToFlatBoundBoxes[0], false }, // RightBankedUp25ToFlat
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
        { &kDiagBlockBrakesSprites[0][0], &kDiagBlockBrakesOffsets[0][0], &kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
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
} // namespace OpenRCT2::InvertedRollerCoaster
