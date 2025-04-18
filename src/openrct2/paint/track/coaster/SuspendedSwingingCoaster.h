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

namespace OpenRCT2::SuspendedSwingingCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            25961,
            25962,
            25961,
            25962,
        }},
        {{
            25963,
            25964,
            25963,
            25964,
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
            {{ 25963, SPR_STATION_INVERTED_BAR_D_SW_NE, }},
            {{ 25964, SPR_STATION_INVERTED_BAR_D_NW_SE, }},
            {{ 25963, SPR_STATION_INVERTED_BAR_D_SW_NE, }},
            {{ 25964, SPR_STATION_INVERTED_BAR_D_NW_SE, }},
        }},
        {{
            {{ 25963, kImageIndexUndefined, }},
            {{ 25964, kImageIndexUndefined, }},
            {{ 25963, kImageIndexUndefined, }},
            {{ 25964, kImageIndexUndefined, }},
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
        {{ { { 0, 6, 29 }, { 32, 20, 2 } }, { { 0, 6, 29 }, { 32, 20, 2 } } }},
        {{ { { 6, 0, 29 }, { 20, 32, 2 } }, { { 6, 0, 29 }, { 20, 32, 2 } } }},
        {{ { { 0, 6, 29 }, { 32, 20, 2 } }, { { 0, 6, 29 }, { 32, 20, 2 } } }},
        {{ { { 6, 0, 29 }, { 20, 32, 2 } }, { { 6, 0, 29 }, { 20, 32, 2 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            25973,
            25974,
            25975,
            25976,
        }},
        {{
            26001,
            26002,
            26003,
            26004,
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
        25989,
        25990,
        25991,
        25992,
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
            25965,
            25966,
            25967,
            25968,
        }},
        {{
            25993,
            25994,
            25995,
            25996,
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
        {{ 25977, kImageIndexUndefined, }},
        {{ 25981, 25978, }},
        {{ 25982, 25979, }},
        {{ 25980, kImageIndexUndefined, }},
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
            {{ 25983, kImageIndexUndefined, }},
            {{ 25987, 25984, }},
            {{ 25988, 25985, }},
            {{ 25986, kImageIndexUndefined, }},
        }},
        {{
            {{ 25983, kImageIndexUndefined, }},
            {{ 25987, 26090, }},
            {{ 25988, 25985, }},
            {{ 25986, kImageIndexUndefined, }},
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
            25969,
            25970,
            25971,
            25972,
        }},
        {{
            25997,
            25998,
            25999,
            26000,
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
            26014,
            kImageIndexUndefined,
            26013,
            26012,
            kImageIndexUndefined,
            26011,
            26010,
        }},
        {{
            26019,
            kImageIndexUndefined,
            26018,
            26017,
            kImageIndexUndefined,
            26016,
            26015,
        }},
        {{
            26024,
            kImageIndexUndefined,
            26023,
            26022,
            kImageIndexUndefined,
            26021,
            26020,
        }},
        {{
            26009,
            kImageIndexUndefined,
            26008,
            26007,
            kImageIndexUndefined,
            26006,
            26005,
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
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            26097,
            kImageIndexUndefined,
            26098,
            26099,
            kImageIndexUndefined,
            26100,
            26101,
        }},
        {{
            26102,
            kImageIndexUndefined,
            26103,
            26104,
            kImageIndexUndefined,
            26105,
            26106,
        }},
        {{
            26107,
            kImageIndexUndefined,
            26108,
            26109,
            kImageIndexUndefined,
            26110,
            26111,
        }},
        {{
            26112,
            kImageIndexUndefined,
            26113,
            26114,
            kImageIndexUndefined,
            26115,
            26116,
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
            { { 6, 0, 29 }, { 20, 32, 11 } },
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
            { { 0, 16, 37 }, { 32, 16, 3 } },
            { { 0, 6, 29 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            26077,
            kImageIndexUndefined,
            26078,
            26079,
            kImageIndexUndefined,
            26080,
            26081,
        }},
        {{
            26082,
            kImageIndexUndefined,
            26083,
            26084,
            kImageIndexUndefined,
            26085,
            26086,
        }},
        {{
            26087,
            kImageIndexUndefined,
            26088,
            26089,
            kImageIndexUndefined,
            26090,
            26091,
        }},
        {{
            26092,
            kImageIndexUndefined,
            26093,
            26094,
            kImageIndexUndefined,
            26095,
            26096,
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
            26139,
            26140,
            26141,
            26142,
        }},
        {{
            26146,
            26145,
            26144,
            26143,
        }},
        {{
            26142,
            26141,
            26140,
            26139,
        }},
        {{
            26143,
            26144,
            26145,
            26146,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
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
            26135,
            26136,
            26137,
            26138,
        }},
        {{
            26150,
            26149,
            26148,
            26147,
        }},
        {{
            26138,
            26137,
            26136,
            26135,
        }},
        {{
            26147,
            26148,
            26149,
            26150,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightOffsets = {{
        {{
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
        {{
            { 0, 6, 29 }, 
            { 0, 0, 29 }, 
            { 0, 6, 29 }, 
            { 0, 6, 29 }, 
        }},
        {{
            { 6, 0, 29 }, 
            { 0, 0, 29 }, 
            { 6, 0, 29 }, 
            { 6, 0, 29 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            26070,
            kImageIndexUndefined,
            26069,
            26068,
        }},
        {{
            26073,
            kImageIndexUndefined,
            26072,
            26071,
        }},
        {{
            26076,
            kImageIndexUndefined,
            26075,
            26074,
        }},
        {{
            26067,
            kImageIndexUndefined,
            26066,
            26065,
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
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            26130,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26129,
        }},
        {{
            26132,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26131,
        }},
        {{
            26134,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26133,
        }},
        {{
            26128,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26127,
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
            { { 6, 0, 29 }, { 20, 32, 11 } },
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
            26119,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26120,
        }},
        {{
            26121,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26122,
        }},
        {{
            26123,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26124,
        }},
        {{
            26125,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26126,
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        26117,
        26118,
        26117,
        26118,
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
    kLeftQuarterHelixLargeUpSprites = {{
        {{
            26054,
            kImageIndexUndefined,
            26053,
            26052,
            kImageIndexUndefined,
            26051,
            26050,
        }},
        {{
            26059,
            kImageIndexUndefined,
            26058,
            26057,
            kImageIndexUndefined,
            26056,
            26055,
        }},
        {{
            26064,
            kImageIndexUndefined,
            26063,
            26062,
            kImageIndexUndefined,
            26061,
            26060,
        }},
        {{
            26049,
            kImageIndexUndefined,
            26048,
            26047,
            kImageIndexUndefined,
            26046,
            26045,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterHelixLargeUpOffsets = {{
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
    kLeftQuarterHelixLargeUpBoundBoxes = {{
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
    kRightQuarterHelixLargeUpSprites = {{
        {{
            26025,
            kImageIndexUndefined,
            26026,
            26027,
            kImageIndexUndefined,
            26028,
            26029,
        }},
        {{
            26030,
            kImageIndexUndefined,
            26031,
            26032,
            kImageIndexUndefined,
            26033,
            26034,
        }},
        {{
            26035,
            kImageIndexUndefined,
            26036,
            26037,
            kImageIndexUndefined,
            26038,
            26039,
        }},
        {{
            26040,
            kImageIndexUndefined,
            26041,
            26042,
            kImageIndexUndefined,
            26043,
            26044,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterHelixLargeUpOffsets = {{
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
    kRightQuarterHelixLargeUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterHelixLargeUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            26167,
            26168,
            26169,
            kImageIndexUndefined,
            26170,
        }},
        {{
            26171,
            26172,
            26173,
            kImageIndexUndefined,
            26174,
        }},
        {{
            26175,
            26176,
            26177,
            kImageIndexUndefined,
            26178,
        }},
        {{
            26179,
            26180,
            26181,
            kImageIndexUndefined,
            26182,
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
            26151,
            26152,
            26153,
            kImageIndexUndefined,
            26154,
        }},
        {{
            26155,
            26156,
            26157,
            kImageIndexUndefined,
            26158,
        }},
        {{
            26159,
            26160,
            26161,
            kImageIndexUndefined,
            26162,
        }},
        {{
            26163,
            26164,
            26165,
            kImageIndexUndefined,
            26166,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                26183,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26184,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26185,
                kImageIndexUndefined,
            }},
            {{
                26186,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26211,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26212,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26213,
                kImageIndexUndefined,
            }},
            {{
                26214,
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
                26195,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26196,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26197,
                kImageIndexUndefined,
            }},
            {{
                26198,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26223,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26224,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26225,
                kImageIndexUndefined,
            }},
            {{
                26226,
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
            26207,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26208,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26209,
            kImageIndexUndefined,
        }},
        {{
            26210,
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
                26187,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26188,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26189,
                kImageIndexUndefined,
            }},
            {{
                26190,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26215,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26216,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26217,
                kImageIndexUndefined,
            }},
            {{
                26218,
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
            26199,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26200,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26201,
            kImageIndexUndefined,
        }},
        {{
            26202,
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
            26203,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            26204,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            26205,
            kImageIndexUndefined,
        }},
        {{
            26206,
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
                26191,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26192,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26193,
                kImageIndexUndefined,
            }},
            {{
                26194,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                26219,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                26220,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                26221,
                kImageIndexUndefined,
            }},
            {{
                26222,
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
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBlockBrakesSprites = {{
        26117,
        26118,
        26117,
        26118,
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0][0], &kUp25Offsets[0], &kUp25BoundBoxes[0], false }, // Up25
        { &kUp60Sprites[0], &kUp60Offsets[0], &kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0], &kUp25ToUp60Offsets[0][0], &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
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
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0], &kLeftQuarterTurn5TilesUp25Offsets[0][0], &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0], &kRightQuarterTurn5TilesUp25Offsets[0][0], &kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], &kSBendLeftOffsets[0][0], &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], &kSBendRightOffsets[0][0], &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], &kLeftQuarterTurn3TilesOffsets[0][0], &kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, false }, // LeftBankedQuarterTurn3Tiles
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
        { &kLeftQuarterHelixLargeUpSprites[0][0], &kLeftQuarterHelixLargeUpOffsets[0][0], &kLeftQuarterHelixLargeUpBoundBoxes[0][0], false }, // LeftQuarterHelixLargeUp
        { &kRightQuarterHelixLargeUpSprites[0][0], &kRightQuarterHelixLargeUpOffsets[0][0], &kRightQuarterHelixLargeUpBoundBoxes[0][0], false }, // RightQuarterHelixLargeUp
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
        { &kLeftEighthToDiagSprites[0][0], &kLeftEighthToDiagOffsets[0][0], &kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0], &kRightEighthToDiagOffsets[0][0], &kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiag
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
} // namespace OpenRCT2::SuspendedSwingingCoaster
