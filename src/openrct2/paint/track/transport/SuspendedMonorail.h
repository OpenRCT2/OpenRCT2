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
#include "../../../ride/TrackPaint.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::SuspendedMonorail
{
    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatSprites = {{
        25853,
        25854,
        25853,
        25854,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatOffsets = {{
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
        { { 0, 6, 32 }, { 32, 20, 3 } },
        { { 6, 0, 32 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            {{ 25853, SPR_STATION_INVERTED_BAR_F_SW_NE, }},
            {{ 25854, SPR_STATION_INVERTED_BAR_F_NW_SE, }},
            {{ 25853, SPR_STATION_INVERTED_BAR_F_SW_NE, }},
            {{ 25854, SPR_STATION_INVERTED_BAR_F_NW_SE, }},
        }},
        {{
            {{ 25853, kImageIndexUndefined, }},
            {{ 25854, kImageIndexUndefined, }},
            {{ 25853, kImageIndexUndefined, }},
            {{ 25854, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kStationOffsets = {{
        {{ { 0, 0, 32 }, { 0, 6, 32 } }},
        {{ { 0, 0, 32 }, { 6, 0, 32 } }},
        {{ { 0, 0, 32 }, { 0, 6, 32 } }},
        {{ { 0, 0, 32 }, { 6, 0, 32 } }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        {{ { { 0, 6, 32 }, { 32, 20, 3 } }, { { 0, 6, 32 }, { 32, 20, 3 } } }},
        {{ { { 6, 0, 32 }, { 20, 32, 3 } }, { { 6, 0, 32 }, { 20, 32, 3 } } }},
        {{ { { 0, 6, 32 }, { 32, 20, 3 } }, { { 0, 6, 32 }, { 32, 20, 3 } } }},
        {{ { { 6, 0, 32 }, { 20, 32, 3 } }, { { 6, 0, 32 }, { 20, 32, 3 } } }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25Sprites = {{
        25945,
        25946,
        25947,
        25948,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25Offsets = {{
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 6, 48 }, { 32, 20, 3 } },
        { { 6, 0, 48 }, { 20, 32, 3 } },
        { { 0, 6, 48 }, { 32, 20, 3 } },
        { { 6, 0, 48 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        25937,
        25938,
        25939,
        25940,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToUp25Offsets = {{
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 40 }, { 32, 20, 3 } },
        { { 6, 0, 40 }, { 20, 32, 3 } },
        { { 0, 6, 40 }, { 32, 20, 3 } },
        { { 6, 0, 40 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        25941,
        25942,
        25943,
        25944,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToFlatOffsets = {{
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
        { 0, 0, 32 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 6, 40 }, { 32, 20, 3 } },
        { { 6, 0, 40 }, { 20, 32, 3 } },
        { { 0, 6, 40 }, { 32, 20, 3 } },
        { { 6, 0, 40 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            25876,
            kImageIndexUndefined,
            25875,
            25874,
            kImageIndexUndefined,
            25873,
            25872,
        }},
        {{
            25881,
            kImageIndexUndefined,
            25880,
            25879,
            kImageIndexUndefined,
            25878,
            25877,
        }},
        {{
            25886,
            kImageIndexUndefined,
            25885,
            25884,
            kImageIndexUndefined,
            25883,
            25882,
        }},
        {{
            25871,
            kImageIndexUndefined,
            25870,
            25869,
            kImageIndexUndefined,
            25868,
            25867,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesOffsets = {{
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 32 }, { 32, 16, 3 } },
            { { 0, 16, 32 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 32 }, { 16, 32, 3 } },
            { { 6, 0, 32 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 32 }, { 16, 32, 3 } },
            { { 16, 16, 32 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 32 }, { 32, 16, 3 } },
            { { 0, 6, 32 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 32 }, { 32, 16, 3 } },
            { { 16, 0, 32 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 32 }, { 16, 32, 3 } },
            { { 6, 0, 32 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 32 }, { 16, 32, 3 } },
            { { 0, 0, 32 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 32 }, { 32, 16, 3 } },
            { { 0, 6, 32 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            25919,
            25920,
            25921,
            25922,
        }},
        {{
            25923,
            25924,
            25925,
            25926,
        }},
        {{
            25922,
            25921,
            25920,
            25919,
        }},
        {{
            25926,
            25925,
            25924,
            25923,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftOffsets = {{
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            { { 0, 0, 32 }, { 32, 26, 3 } },
            { { 0, 6, 32 }, { 32, 26, 3 } },
            { { 0, 6, 32 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            { { 0, 0, 32 }, { 26, 32, 3 } },
            { { 6, 0, 32 }, { 26, 32, 3 } },
            { { 6, 0, 32 }, { 20, 32, 3 } },
        }},
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            { { 0, 6, 32 }, { 32, 26, 3 } },
            { { 0, 0, 32 }, { 32, 26, 3 } },
            { { 0, 6, 32 }, { 32, 20, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            { { 6, 0, 32 }, { 26, 32, 3 } },
            { { 0, 0, 32 }, { 26, 32, 3 } },
            { { 6, 0, 32 }, { 20, 32, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            25927,
            25928,
            25929,
            25930,
        }},
        {{
            25931,
            25932,
            25933,
            25934,
        }},
        {{
            25930,
            25929,
            25928,
            25927,
        }},
        {{
            25934,
            25933,
            25932,
            25931,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightOffsets = {{
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            25860,
            kImageIndexUndefined,
            25859,
            25858,
        }},
        {{
            25863,
            kImageIndexUndefined,
            25862,
            25861,
        }},
        {{
            25866,
            kImageIndexUndefined,
            25865,
            25864,
        }},
        {{
            25857,
            kImageIndexUndefined,
            25856,
            25855,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesOffsets = {{
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            {},
            { { 16, 0, 32 }, { 16, 16, 3 } },
            { { 6, 0, 32 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 32 }, { 16, 16, 3 } },
            { { 0, 6, 32 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 32 }, { 16, 16, 3 } },
            { { 6, 0, 32 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            {},
            { { 16, 16, 32 }, { 16, 16, 3 } },
            { { 0, 6, 32 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            25903,
            25904,
            25905,
            kImageIndexUndefined,
            25906,
        }},
        {{
            25907,
            25908,
            25909,
            kImageIndexUndefined,
            25910,
        }},
        {{
            25911,
            25912,
            25913,
            kImageIndexUndefined,
            25914,
        }},
        {{
            25915,
            25916,
            25917,
            kImageIndexUndefined,
            25918,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagOffsets = {{
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            { { 0, 0, 32 }, { 32, 16, 3 } },
            { { 0, 16, 32 }, { 16, 16, 3 } },
            {},
            { { 16, 16, 32 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            { { 0, 0, 32 }, { 16, 34, 3 } },
            { { 16, 16, 32 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 32 }, { 18, 16, 3 } },
        }},
        {{
            { { 0, 6, 32 }, { 32, 20, 3 } },
            { { 0, 16, 32 }, { 32, 16, 3 } },
            { { 16, 0, 32 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 32 }, { 16, 16, 3 } },
        }},
        {{
            { { 6, 0, 32 }, { 20, 32, 3 } },
            { { 16, 0, 32 }, { 16, 32, 3 } },
            { { 0, 0, 32 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 32 }, { 16, 16, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            25887,
            25888,
            25889,
            kImageIndexUndefined,
            25890,
        }},
        {{
            25891,
            25892,
            25893,
            kImageIndexUndefined,
            25894,
        }},
        {{
            25895,
            25896,
            25897,
            kImageIndexUndefined,
            25898,
        }},
        {{
            25899,
            25900,
            25901,
            kImageIndexUndefined,
            25902,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagOffsets = {{
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
        {{
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            { 0, 0, 32 }, 
            {},
            { 0, 0, 32 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatSprites = {{
        {{
            kImageIndexUndefined,
            25935,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25936,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            25935,
            kImageIndexUndefined,
        }},
        {{
            25936,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatOffsets = {{
        {{
            {},
            { -16, -16, 32 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 32 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 32 }, 
            {},
        }},
        {{
            { -16, -16, 32 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatBoundBoxes = {{
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
    kDiagUp25Sprites = {{
        {{
            kImageIndexUndefined,
            25957,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25958,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            25959,
            kImageIndexUndefined,
        }},
        {{
            25960,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25Offsets = {{
        {{
            {},
            { -16, -16, 32 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 32 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 32 }, 
            {},
        }},
        {{
            { -16, -16, 32 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25BoundBoxes = {{
        {{
            {},
            { { -16, -16, 48 }, { 32, 32, 3 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 48 }, { 32, 32, 3 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 48 }, { 32, 32, 3 } },
            {},
        }},
        {{
            { { -16, -16, 48 }, { 32, 32, 3 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            25949,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25950,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            25951,
            kImageIndexUndefined,
        }},
        {{
            25952,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25Offsets = {{
        {{
            {},
            { -16, -16, 32 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 32 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 32 }, 
            {},
        }},
        {{
            { -16, -16, 32 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25BoundBoxes = {{
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
    kDiagUp25ToFlatSprites = {{
        {{
            kImageIndexUndefined,
            25953,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            25954,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            25955,
            kImageIndexUndefined,
        }},
        {{
            25956,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatOffsets = {{
        {{
            {},
            { -16, -16, 32 }, 
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { -16, -16, 32 }, 
        }},
        {{
            {},
            {},
            { -16, -16, 32 }, 
            {},
        }},
        {{
            { -16, -16, 32 }, 
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatBoundBoxes = {{
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], &kStationOffsets[0][0], &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0], &kUp25Offsets[0], &kUp25BoundBoxes[0], false }, // Up25
        { nullptr, nullptr, nullptr, false }, // Up60
        { &kFlatToUp25Sprites[0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { nullptr, nullptr, nullptr, false }, // Up25ToUp60
        { nullptr, nullptr, nullptr, false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0], &kUp25ToFlatOffsets[0], &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
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
        { nullptr, nullptr, nullptr, false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { nullptr, nullptr, nullptr, false }, // Up60ToUp90
        { nullptr, nullptr, nullptr, true }, // Down90ToDown60
        { nullptr, nullptr, nullptr, false }, // Up90ToUp60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &kLeftEighthToDiagSprites[0][0], &kLeftEighthToDiagOffsets[0][0], &kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0], &kRightEighthToDiagOffsets[0][0], &kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0], &kDiagFlatOffsets[0][0], &kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0], &kDiagUp25Offsets[0][0], &kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToUp60
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0], &kDiagUp25ToFlatOffsets[0][0], &kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
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
} // namespace OpenRCT2::SuspendedMonorail
