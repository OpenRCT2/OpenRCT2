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

namespace OpenRCT2::GoKarts
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatSprites = {{
        {{ 20752, 20754, }},
        {{ 20753, 20755, }},
        {{ 20752, 20754, }},
        {{ 20753, 20755, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 3 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 3 } } }},
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 3 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 3 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kEndStationSprites = {{
        {{
            {{ 20756, 20760, 20808, 20814, }},
            {{ 20757, 20761, 20810, 20816, }},
            {{ 20758, 20762, 20811, 20817, }},
            {{ 20759, 20763, 20812, 20818, }},
        }},
        {{
            {{ 20756, 20760, 20808, 20815, }},
            {{ 20757, 20761, 20810, 20816, }},
            {{ 20758, 20762, 20811, 20817, }},
            {{ 20759, 20763, 20812, 20819, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kEndStationBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 3 } }, { { 1, 1, 4 }, { 3, 3, 13 } }, { { 1, 28, 4 }, { 3, 3, 13 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 3 } }, { { 1, 28, 4 }, { 3, 3, 13 } }, { { 28, 28, 4 }, { 3, 3, 13 } } }},
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 3 } }, { { 28, 1, 4 }, { 3, 3, 13 } }, { { 28, 28, 4 }, { 3, 3, 13 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 3 } }, { { 1, 1, 4 }, { 3, 3, 13 } }, { { 28, 1, 4 }, { 3, 3, 13 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kStationSprites = {{
        {{ 20764, 20768, }},
        {{ 20765, 20769, }},
        {{ 20766, 20770, }},
        {{ 20767, 20771, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 3 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 3 } } }},
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 3 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 3 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25Sprites = {{
        {{ 20780, 20792, }},
        {{ 20781, 20793, }},
        {{ 20782, 20794, }},
        {{ 20783, 20795, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 11 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 11 } } }},
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 11 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 11 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60Sprites = {{
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 16, SPR_G2_GO_KARTS_TRACK_STEEP + 17, }},
        {{ kImageIndexUndefined, SPR_G2_GO_KARTS_TRACK_STEEP + 18, }},
        {{ kImageIndexUndefined, SPR_G2_GO_KARTS_TRACK_STEEP + 19, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 20, SPR_G2_GO_KARTS_TRACK_STEEP + 21, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 86 }, { 32, 28, 1 } } }},
        {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 4, 28, 0 }, { 24, 2, 77 } } }},
        {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 28, 4, 0 }, { 2, 24, 77 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 86 }, { 28, 32, 1 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        {{ 20772, 20784, }},
        {{ 20773, 20785, }},
        {{ 20774, 20786, }},
        {{ 20775, 20787, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 11 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 11 } } }},
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 11 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 11 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 0, SPR_G2_GO_KARTS_TRACK_STEEP + 1, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 2, SPR_G2_GO_KARTS_TRACK_STEEP + 3, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 4, SPR_G2_GO_KARTS_TRACK_STEEP + 5, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 6, SPR_G2_GO_KARTS_TRACK_STEEP + 7, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 0 } }, { { 0, 2, 54 }, { 32, 28, 1 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 4, 28, 0 }, { 24, 2, 56 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 28, 4, 0 }, { 2, 24, 56 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 0 } }, { { 2, 0, 54 }, { 28, 32, 1 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Sprites = {{
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 8, SPR_G2_GO_KARTS_TRACK_STEEP + 9, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 10, SPR_G2_GO_KARTS_TRACK_STEEP + 11, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 12, SPR_G2_GO_KARTS_TRACK_STEEP + 13, }},
        {{ SPR_G2_GO_KARTS_TRACK_STEEP + 14, SPR_G2_GO_KARTS_TRACK_STEEP + 15, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 0 } }, { { 0, 2, 54 }, { 32, 28, 1 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 4, 28, 0 }, { 24, 2, 56 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 28, 4, 0 }, { 2, 24, 56 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 0 } }, { { 2, 0, 54 }, { 28, 32, 1 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        {{ 20776, 20788, }},
        {{ 20777, 20789, }},
        {{ 20778, 20790, }},
        {{ 20779, 20791, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 11 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 11 } } }},
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 29, 2 }, { 32, 1, 11 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 29, 0, 2 }, { 1, 32, 11 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 3>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 0, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 1, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 2, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 3, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 4, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 5, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 6, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 7, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 8, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 9, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 10, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 11, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 13, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 14, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 15, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 16, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 17, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 18, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 19, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 20, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 21, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 22, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 23, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 24, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 25, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 26, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 27, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 28, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 29, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 30, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 31, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 32, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 33, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 34, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 35, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 36, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 37, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 38, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 39, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 40, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 41, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 42, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 43, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 44, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 45, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 46, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 47, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 48, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 49, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 50, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 51, SPR_G2_GO_KARTS_TRACK_MEDIUM_CURVE + 52, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            {{ { { 0, 2, 1 }, { 32, 26, 1 } }, { { 0, 28, 2 }, { 32, 1, 16 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 22 }, { 16, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 1 }, { 16, 16, 1 } }, { { 7, 31, 2 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 28, 0, 1 }, { 1, 32, 3 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 28, 0, 1 }, { 1, 16, 3 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } }, { { 32, 32, 0 }, { 1, 1, 1 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 28, 1 }, { 16, 1, 3 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 28, 1 }, { 32, 1, 3 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 0, 1 }, { 16, 16, 1 } }, { { 31, 7, 2 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 22 }, { 16, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 22 }, { 16, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 2, 0, 1 }, { 26, 32, 1 } }, { { 28, 0, 2 }, { 1, 32, 16 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 1 }, { 16, 16, 1 } }, { { 0, 16, 2 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 0, 2 }, { 16, 32, 1 } }, { { 16, 0, 22 }, { 16, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 1 }, { 16, 16, 1 } }, { { 16, 16, 2 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 1 }, { 16, 16, 1 } }, { { 16, 0, 2 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 2 }, { 32, 16, 1 } }, { { 0, 16, 22 }, { 32, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 0, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 2, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 3, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 4, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 5, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 6, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 7, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 8, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 9, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 10, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 12, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 13, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 14, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 15, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 16, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 17, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 18, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 19, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 20, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 21, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 22, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 23, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 24, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 25, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 26, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 27, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 28, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 29, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 30, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 31, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 32, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 33, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 34, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 35, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 36, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 37, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 38, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 39, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25BoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 38 }, { 32, 16, 1 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, { { 16, 0, 38 }, { 16, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 16, 0 }, { 32, 32, 1 } }, { { 0, 16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 8 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, { { 0, 16, 38 }, { 32, 16, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, { { 0, 0, 38 }, { 16, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, { { 16, 0, 38 }, { 16, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, { { 0, 16, 38 }, { 32, 16, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 40, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 41, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 42, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 43, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 44, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 46, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 47, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 48, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 49, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 50, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 51, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 52, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 53, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 54, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 55, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 56, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 57, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 58, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 59, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 60, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 61, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 62, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 63, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 64, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 65, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 66, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 67, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 68, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 69, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 70, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 71, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 72, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 73, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 74, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 75, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 76, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 77, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 78, SPR_G2_GO_KARTS_TRACK_GENTLE_MEDIUM_CURVE + 79, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 3>, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 0, SPR_G2_GO_KARTS_TRACK_S_BEND + 1, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 2, SPR_G2_GO_KARTS_TRACK_S_BEND + 3, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 4, SPR_G2_GO_KARTS_TRACK_S_BEND + 5, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 6, SPR_G2_GO_KARTS_TRACK_S_BEND + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 8, SPR_G2_GO_KARTS_TRACK_S_BEND + 9, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 10, SPR_G2_GO_KARTS_TRACK_S_BEND + 11, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 12, SPR_G2_GO_KARTS_TRACK_S_BEND + 13, SPR_G2_GO_KARTS_TRACK_S_BEND + 14, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 15, SPR_G2_GO_KARTS_TRACK_S_BEND + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 6, SPR_G2_GO_KARTS_TRACK_S_BEND + 7, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 4, SPR_G2_GO_KARTS_TRACK_S_BEND + 5, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 2, SPR_G2_GO_KARTS_TRACK_S_BEND + 3, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 0, SPR_G2_GO_KARTS_TRACK_S_BEND + 1, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 15, SPR_G2_GO_KARTS_TRACK_S_BEND + 16, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 12, SPR_G2_GO_KARTS_TRACK_S_BEND + 13, SPR_G2_GO_KARTS_TRACK_S_BEND + 14, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 10, SPR_G2_GO_KARTS_TRACK_S_BEND + 11, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 8, SPR_G2_GO_KARTS_TRACK_S_BEND + 9, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 26, 1 } }, { { 0, 0, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 26, 1 } }, { { 0, 6, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 26, 29, 1 } }, { { 0, 0, 22 }, { 26, 29, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 6, 0, 0 }, { 26, 32, 1 } }, { { 6, 0, 22 }, { 26, 32, 1 } }, { { 32, 32, 22 }, { 1, 1, 1 } } }},
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 26, 1 } }, { { 0, 6, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 26, 1 } }, { { 0, 0, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 6, 0, 0 }, { 26, 32, 1 } }, { { 6, 0, 22 }, { 26, 32, 1 } }, { { 32, 32, 22 }, { 1, 1, 1 } } }},
            {{ { { 0, 0, 0 }, { 26, 29, 1 } }, { { 0, 0, 22 }, { 26, 29, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 3>, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 17, SPR_G2_GO_KARTS_TRACK_S_BEND + 18, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 19, SPR_G2_GO_KARTS_TRACK_S_BEND + 20, SPR_G2_GO_KARTS_TRACK_S_BEND + 21, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 22, SPR_G2_GO_KARTS_TRACK_S_BEND + 23, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 24, SPR_G2_GO_KARTS_TRACK_S_BEND + 25, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 26, SPR_G2_GO_KARTS_TRACK_S_BEND + 27, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 28, SPR_G2_GO_KARTS_TRACK_S_BEND + 29, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 30, SPR_G2_GO_KARTS_TRACK_S_BEND + 31, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 32, SPR_G2_GO_KARTS_TRACK_S_BEND + 33, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 24, SPR_G2_GO_KARTS_TRACK_S_BEND + 25, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 22, SPR_G2_GO_KARTS_TRACK_S_BEND + 23, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 19, SPR_G2_GO_KARTS_TRACK_S_BEND + 20, SPR_G2_GO_KARTS_TRACK_S_BEND + 21, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 17, SPR_G2_GO_KARTS_TRACK_S_BEND + 18, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 32, SPR_G2_GO_KARTS_TRACK_S_BEND + 33, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 30, SPR_G2_GO_KARTS_TRACK_S_BEND + 31, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 28, SPR_G2_GO_KARTS_TRACK_S_BEND + 29, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_S_BEND + 26, SPR_G2_GO_KARTS_TRACK_S_BEND + 27, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 0, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 2, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 3, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 4, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 5, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 6, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 7, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 8, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 9, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 10, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 12, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 13, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 14, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 15, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 16, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 17, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 18, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 19, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 20, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 21, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 22, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 23, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 24, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 25, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 26, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 27, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 28, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 29, }},
            {{ SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 30, SPR_G2_GO_KARTS_TRACK_SMALL_CURVE + 31, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            {{ { { 0, 2, 1 }, { 32, 26, 1 } }, { { 0, 28, 2 }, { 32, 1, 16 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 7, 3, 2 }, { 1, 1, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 22 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 22 }, { 20, 32, 1 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 1 }, { 16, 16, 1 } }, { { 15, 15, 2 }, { 1, 1, 1 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 22 }, { 32, 20, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 3, 7, 1 }, { 1, 1, 1 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 22 }, { 16, 16, 1 } } }},
            {{ { { 2, 0, 1 }, { 26, 32, 1 } }, { { 28, 0, 2 }, { 1, 32, 16 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 8, 8, 1 } }, { { 7, 7, 1 }, { 1, 1, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 22 }, { 32, 26, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 0, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 2, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 3, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 4, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 5, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 6, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 7, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 8, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 10, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 12, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 14, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 15, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 16, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 17, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 18, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 19, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 20, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 21, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 22, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 23, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 24, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 25, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25BoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 0 } }, { { 0, 16, 22 }, { 16, 16, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 0 } }, { { 16, 0, 22 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 0 } }, { { 0, 0, 22 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 0 } }, { { 0, 16, 22 }, { 16, 16, 1 } } }},
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 16, 0 }, { 32, 32, 0 } }, { { 16, 16, 22 }, { 32, 32, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 26, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 27, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 28, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 29, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 30, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 31, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 32, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 33, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 34, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 35, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 36, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 37, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 38, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 39, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 40, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 41, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 42, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 43, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 44, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 45, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 46, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 47, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 48, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 49, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 50, SPR_G2_GO_KARTS_TRACK_GENTLE_SMALL_CURVE + 51, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn3TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileSprites = {{
        {{ 20797, 20801, 20805, }},
        {{ 20798, 20802, 20806, }},
        {{ 20799, 20803, 20807, }},
        {{ 20796, 20800, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 29, 2, 2 }, { 1, 1, 3 } }, { { 14, 29, 2 }, { 16, 1, 3 } } }},
        {{ { { 0, 0, 0 }, { 30, 30, 1 } }, { { 2, 29, 2 }, { 16, 1, 3 } }, { { 29, 2, 2 }, { 1, 16, 3 } } }},
        {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 2, 2 }, { 1, 1, 3 } }, { { 29, 14, 2 }, { 1, 16, 3 } } }},
        {{ { { 8, 8, 0 }, { 24, 24, 1 } }, { { 29, 29, 2 }, { 1, 1, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 0, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 2, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 3, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 4, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 5, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 6, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 7, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 8, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 9, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 10, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 12, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 13, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 14, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 15, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 16, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 17, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 18, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 19, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 20, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 21, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 22, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 23, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 24, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 25, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 26, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 27, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 28, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 29, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 30, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 31, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseBoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 38 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 38 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 4, 28, -16 }, { 24, 2, 56 } } }},
        }},
        {{
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 38 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 28, 4, -16 }, { 2, 24, 56 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 32, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 33, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 34, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 35, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 36, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 37, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 38, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 39, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 40, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 41, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 42, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 43, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 44, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 45, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 46, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 47, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 48, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 49, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 50, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 51, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 52, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 53, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 54, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 55, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 56, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 57, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 58, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 59, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 60, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 61, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 62, SPR_G2_GO_KARTS_TRACK_FLAT_TO_STEEP + 63, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseBoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 4, 28, -16 }, { 24, 2, 56 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 28, 4, -16 }, { 2, 24, 56 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 70 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 46 }, { 32, 28, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 28, 1 } }, { { 0, 2, 30 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 70 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 46 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 28, 32, 1 } }, { { 2, 0, 30 }, { 28, 32, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 3>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 0, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 1, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 2, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 3, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 4, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 5, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 6, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 7, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 8, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 10, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 11, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 12, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 13, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 14, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 15, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 16, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 17, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 18, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 19, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 20, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 21, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 22, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 23, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 24, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 25, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 26, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 27, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 28, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 29, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 30, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 31, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 32, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 33, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 34, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 35, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 36, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 37, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 38, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 39, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 40, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            {{ { { 0, 2, 1 }, { 32, 26, 1 } }, { { 0, 28, 2 }, { 32, 1, 16 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 22 }, { 32, 28, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 22 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 22 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 22 }, { 28, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 22 }, { 28, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 8, 8, 1 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 1, 33, 1 }, { 1, 1, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 22 }, { 32, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 22 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 22 }, { 26, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 32, 32, 1 }, { 1, 1, 1 } }, { { 8, 0, 0 }, { 24, 32, 1 } }, { { 8, 0, 22 }, { 24, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 15, 15, 0 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, { { 16, 16, 22 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 1, 0 }, { 1, 1, 1 } }, { { 30, 31, 0 }, { 1, 1, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 3>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 41, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 42, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 43, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 44, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 45, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 46, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 47, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 48, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 49, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 50, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 51, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 52, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 53, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 54, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 55, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 56, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 57, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 58, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 59, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 60, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 61, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 62, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 63, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 64, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 65, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 66, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 67, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 68, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 69, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 70, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 71, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 72, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 73, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 74, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 75, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 76, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 77, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 78, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 79, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 80, SPR_G2_GO_KARTS_TRACK_LARGE_CURVE + 81, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 3>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 5, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 3, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 1, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatBoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 22 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 22 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 22 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 8, SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 10, SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 12, SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 8, SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 10, SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 12, SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_DIAGONAL + 15, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 86 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 86 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 86 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 8 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 8, SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 10, SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 12, SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_FLAT_TO_GENTLE_DIAGONAL + 15, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 8, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 10, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 12, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_STEEP_DIAGONAL + 15, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 8 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 8, SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 10, SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 12, SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_STEEP_TO_GENTLE_DIAGONAL + 15, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 54 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 54 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 32 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatSprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 0, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 2, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 4, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 5, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 6, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 8, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 9, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 10, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 12, SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 13, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 14, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_TO_FLAT_DIAGONAL + 15, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatBoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 1 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 30 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 30 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 9 }, { 32, 32, 1 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 0, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 1, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 2, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 3, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 4, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 5, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 6, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 7, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 8, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 9, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 10, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 11, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 12, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 13, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 14, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 15, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 16, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 17, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 18, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 19, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 20, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 21, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 22, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 23, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 24, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 25, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 26, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 27, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 28, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 29, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 30, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 31, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25BoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, { { 0, 0, 38 }, { 32, 16, 1 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 0 } }, { { 0, 16, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, { { 16, 16, 38 }, { 16, 16, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 16, 34, 1 } }, { { 0, 0, 38 }, { 16, 34, 1 } } }},
            {{ { { 16, 16, 0 }, { 32, 32, 1 } }, { { 16, 16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 64, 32, 1 } }, { { 0, 0, 38 }, { 64, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, { { 0, 0, 38 }, { 16, 16, 1 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, { { 16, 0, 38 }, { 16, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 32, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 33, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 34, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 35, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 36, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 37, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 38, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 39, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 40, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 41, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 42, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 43, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 44, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 46, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 47, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 48, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 49, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 50, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 51, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 52, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 53, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 54, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 55, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 56, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 57, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 58, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 59, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 60, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 61, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 62, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 63, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagUp25BoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 64, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 65, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 66, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 67, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 68, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 69, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 70, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 71, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 72, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 73, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 74, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 75, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 76, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 77, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 78, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 79, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 80, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 81, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 82, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 83, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 84, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 85, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 86, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 87, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 88, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 89, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 90, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 91, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 92, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 93, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 94, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 95, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 16, 16, 0 }, { 32, 32, 1 } }, { { 16, 16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } }},
            {{ { { 0, 0, 0 }, { 48, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 16, 16, 0 } }, { { 0, 0, 38 }, { 16, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightEighthToOrthogonalUp25Sprites = {{
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 96, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 97, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 98, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 99, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 100, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 101, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 102, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 103, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 104, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 105, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 106, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 107, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 108, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 109, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 110, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 111, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 112, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 113, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 114, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 115, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 116, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 117, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 118, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 119, }},
        }},
        {{
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 120, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 121, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 122, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 123, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 124, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 125, }},
            {{ SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 126, SPR_G2_GO_KARTS_TRACK_GENTLE_LARGE_CURVE + 127, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kRightEighthToOrthogonalUp25BoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 2, 0, 0 }, { 26, 32, 1 } }, { { 2, 0, 38 }, { 26, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, { { 0, 16, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } }},
            {{ { { 0, 0, 0 }, { 32, 28, 1 } }, { { 0, 0, 38 }, { 32, 28, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 0 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 16, 16, 0 }, { 32, 32, 1 } }, { { 16, 16, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 28, 32, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
            {{ { { 0, 0, 0 }, { 28, 48, 1 } }, { { 0, 0, 38 }, { 28, 32, 1 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 16, 16, 0 } }, { { 0, 0, 38 }, { 16, 16, 1 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, { { 16, 0, 38 }, { 16, 16, 1 } } }},
            {{ { { 0, 0, 0 }, { 0, 0, 0 } }, { { 0, 0, 0 }, { 0, 0, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 32, 1 } }, { { 0, 0, 38 }, { 32, 32, 1 } } }},
            {{ { { 0, 2, 0 }, { 32, 26, 1 } }, { { 0, 2, 38 }, { 32, 26, 1 } } }},
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // Flat
        { &kEndStationSprites[0][0][0], nullptr, &kEndStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0], nullptr, &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0], nullptr, &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0][0], nullptr, &kUp25BoundBoxes[0][0], false }, // Up25
        { &kUp60Sprites[0][0], nullptr, &kUp60BoundBoxes[0][0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], nullptr, &kFlatToUp25BoundBoxes[0][0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0], nullptr, &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0], nullptr, &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], nullptr, &kUp25ToFlatBoundBoxes[0][0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0][0], nullptr, &kLeftQuarterTurn5TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn5Tiles
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
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0][0], nullptr, &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0][0], false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0][0], nullptr, &kRightQuarterTurn5TilesUp25BoundBoxes[0][0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0][0], nullptr, &kSBendLeftBoundBoxes[0][0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0][0], nullptr, &kSBendRightBoundBoxes[0][0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, false }, // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &kLeftQuarterTurn3TilesUp25Sprites[0][0][0], nullptr, &kLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0][0], nullptr, &kRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25
        { &kLeftQuarterTurn1TileSprites[0][0], nullptr, &kLeftQuarterTurn1TileBoundBoxes[0][0], false }, // LeftQuarterTurn1Tile
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
        { &kFlatToUp60LongBaseSprites[0][0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0][0], false }, // Up60ToFlatLongBase
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
        { &kLeftEighthToDiagSprites[0][0][0], nullptr, &kLeftEighthToDiagBoundBoxes[0][0][0], false }, // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0][0], nullptr, &kRightEighthToDiagBoundBoxes[0][0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0], nullptr, &kDiagFlatBoundBoxes[0][0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0], nullptr, &kDiagUp25BoundBoxes[0][0][0], false }, // DiagUp25
        { &kDiagUp60Sprites[0][0][0], nullptr, &kDiagUp60BoundBoxes[0][0][0], false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], nullptr, &kDiagFlatToUp25BoundBoxes[0][0][0], false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0][0], nullptr, &kDiagUp25ToUp60BoundBoxes[0][0][0], false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0][0], nullptr, &kDiagUp60ToUp25BoundBoxes[0][0][0], false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0], nullptr, &kDiagUp25ToFlatBoundBoxes[0][0][0], false }, // DiagUp25ToFlat
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
        { nullptr, nullptr, nullptr, true }, // InvertedFlatToDown90QuarterLoop
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
        { &kLeftEighthToDiagUp25Sprites[0][0][0], nullptr, &kLeftEighthToDiagUp25BoundBoxes[0][0][0], false }, // LeftEighthToDiagUp25
        { &kRightEighthToDiagUp25Sprites[0][0][0], nullptr, &kRightEighthToDiagUp25BoundBoxes[0][0][0], false }, // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToDiagDown25
        { &kLeftEighthToOrthogonalUp25Sprites[0][0][0], nullptr, &kLeftEighthToOrthogonalUp25BoundBoxes[0][0][0], false }, // LeftEighthToOrthogonalUp25
        { &kRightEighthToOrthogonalUp25Sprites[0][0][0], nullptr, &kRightEighthToOrthogonalUp25BoundBoxes[0][0][0], false }, // RightEighthToOrthogonalUp25
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
} // namespace OpenRCT2::GoKarts
