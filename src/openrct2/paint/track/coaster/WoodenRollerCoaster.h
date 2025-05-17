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

namespace OpenRCT2::WoodenRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            {{ 23753, 24619, }},
            {{ 23754, 24620, }},
            {{ 23753, 24619, }},
            {{ 23754, 24620, }},
        }},
        {{
            {{ 23749, 24615, }},
            {{ 23750, 24616, }},
            {{ 23751, 24617, }},
            {{ 23752, 24618, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections> kFlatOffsets = { {
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kFlatBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>, 2>
    kEndStationSprites = {{
            {{
        {{
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_OPEN_SW_NE,
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_OPEN_NW_SE,
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_OPEN_SW_NE,
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_OPEN_NW_SE,
        }},
        {{
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_CLOSED_SW_NE,
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_CLOSED_NW_SE,
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_CLOSED_SW_NE,
            SPR_G2_WOODEN_RC_STATION_BLOCK_BRAKE_CLOSED_NW_SE,
        }},
        }},
            {{
        {{
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_OPEN_SW_NE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_OPEN_NW_SE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_OPEN_SW_NE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_OPEN_NW_SE,
        }},
        {{
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_CLOSED_SW_NE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_CLOSED_NW_SE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_CLOSED_SW_NE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_BLOCK_BRAKE_CLOSED_NW_SE,
        }},
        }},
    }};
    // clang-format on

    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections> kEndStationOffsets = { {
        { 0, 2, 0 },
        { 2, 0, 0 },
        { 0, 2, 0 },
        { 2, 0, 0 },
    } };

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kEndStationBoundBoxes = { {
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            23973,
            23974,
            23973,
            23974,
        }},
        {{
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_SW_NE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_NW_SE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_SW_NE,
            SPR_G2_WOODEN_RC_STATION_NO_PLATFORM_NW_SE,
        }},
    }};
    // clang-format on

    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections> kStationOffsets = { {
        { 0, 2, 0 },
        { 2, 0, 0 },
        { 0, 2, 0 },
        { 2, 0, 0 },
    } };

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kStationBoundBoxes = { {
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
        { { 0, 2, 0 }, { 32, 27, 2 } },
        { { 2, 0, 0 }, { 27, 32, 2 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            {{ 23545, 24411, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23546, 24412, 23569, 24435, }},
            {{ 23547, 24413, 23570, 24436, }},
            {{ 23548, 24414, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23579, 24445, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23580, 24446, 23603, 24469, }},
            {{ 23581, 24447, 23604, 24470, }},
            {{ 23582, 24448, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60Sprites = {{
        {{
            {{ 23561, 24427, }},
            {{ 23562, 24428, }},
            {{ 23563, 24429, }},
            {{ 23564, 24430, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_60_SWNE_CHAINED + 0, 24427, }},
            {{ SPR_G2_WOODEN_RC_60_SWNE_CHAINED_BACK + 0, 24428, }},
            {{ SPR_G2_WOODEN_RC_60_SENW_CHAINED_BACK + 0, 24429, }},
            {{ SPR_G2_WOODEN_RC_60_SENW_CHAINED + 0, 24430, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kUp60BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 4, 28, -16 }, { 24, 2, 93 } }, { { 4, 28, -16 }, { 24, 2, 93 } } } },
        { { { { 28, 4, -16 }, { 2, 24, 93 } }, { { 28, 4, -16 }, { 2, 24, 93 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            {{ 23537, 24403, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23538, 24404, 23565, 24431, }},
            {{ 23539, 24405, 23566, 24432, }},
            {{ 23540, 24406, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23571, 24437, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23572, 24438, 23599, 24465, }},
            {{ 23573, 24439, 23600, 24466, }},
            {{ 23574, 24440, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kFlatToUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kUp25ToUp60Sprites = {{
        {{
            {{ 23549, 24415, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23551, 24417, 23550, 24416, }},
            {{ 23553, 24419, 23552, 24418, }},
            {{ 23554, 24420, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_25_60_SWNE_CHAINED + 0, 24415, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23551, 24417, SPR_G2_WOODEN_RC_25_60_SENW_CHAINED_BACK + 0, 24416, }},
            {{ 23553, 24419, SPR_G2_WOODEN_RC_25_60_SWNE_CHAINED_BACK + 0, 24418, }},
            {{ SPR_G2_WOODEN_RC_25_60_SENW_CHAINED + 0, 24420, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25ToUp60BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 4, 28, 2 }, { 24, 2, 43 } },
            { { 4, 28, 2 }, { 24, 2, 43 } },
            { { 4, 0, 0 }, { 2, 32, 43 } },
            { { 4, 0, 0 }, { 2, 32, 43 } } } },
        { { { { 28, 4, 2 }, { 2, 24, 43 } },
            { { 28, 4, 2 }, { 2, 24, 43 } },
            { { 0, 4, 0 }, { 32, 2, 43 } },
            { { 0, 4, 0 }, { 32, 2, 43 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kUp60ToUp25Sprites = {{
        {{
            {{ 23555, 24421, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23557, 24423, 23556, 24422, }},
            {{ 23559, 24425, 23558, 24424, }},
            {{ 23560, 24426, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_60_25_SWNE_CHAINED + 0, 24421, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23557, 24423, SPR_G2_WOODEN_RC_60_25_SWNE_CHAINED_BACK + 0, 24422, }},
            {{ 23559, 24425, SPR_G2_WOODEN_RC_60_25_SENW_CHAINED_BACK + 0, 24424, }},
            {{ SPR_G2_WOODEN_RC_60_25_SENW_CHAINED + 0, 24426, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp60ToUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 4, 28, 2 }, { 24, 2, 43 } },
            { { 4, 28, 2 }, { 24, 2, 43 } },
            { { 4, 0, 0 }, { 2, 32, 43 } },
            { { 4, 0, 0 }, { 2, 32, 43 } } } },
        { { { { 28, 4, 2 }, { 2, 24, 43 } },
            { { 28, 4, 2 }, { 2, 24, 43 } },
            { { 0, 4, 0 }, { 32, 2, 43 } },
            { { 0, 4, 0 }, { 32, 2, 43 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            {{ 23541, 24407, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23542, 24408, 23567, 24433, }},
            {{ 23543, 24409, 23568, 24434, }},
            {{ 23544, 24410, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23575, 24441, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23576, 24442, 23601, 24467, }},
            {{ 23577, 24443, 23602, 24468, }},
            {{ 23578, 24444, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25ToFlatBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesSprites = {{
        {{
            {{ 23605, 24471, 23625, 24491, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23606, 24472, 23626, 24492, }},
            {{ 23607, 24473, 23627, 24493, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23608, 24474, 23628, 24494, }},
            {{ 23609, 24475, 23629, 24495, }},
        }},
        {{
            {{ 23610, 24476, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23611, 24477, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23612, 24478, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23613, 24479, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23614, 24480, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23615, 24481, 23630, 24496, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23616, 24482, 23631, 24497, }},
            {{ 23617, 24483, 23632, 24498, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23618, 24484, 23633, 24499, }},
            {{ 23619, 24485, 23634, 24500, }},
        }},
        {{
            {{ 23620, 24486, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23621, 24487, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23622, 24488, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23623, 24489, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23624, 24490, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 7>, kNumOrthogonalDirections>
        kRightQuarterTurn5TilesOffsets = { {
            { {
                { { { 0, 2, 0 }, { 0, 2, 0 }, { 0, 2, 0 }, { 0, 2, 0 } } },
                { { {}, {}, {}, {} } },
                { { { 0, 16, 0 }, { 0, 16, 0 }, { 0, 16, 0 }, { 0, 16, 0 } } },
                { { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { { 16, 0, 0 }, { 16, 0, 0 }, { 16, 0, 0 }, { 16, 0, 0 } } },
                { { { 2, 0, 0 }, { 2, 0, 0 }, { 2, 0, 0 }, { 2, 0, 0 } } },
            } },
            { {
                { { { 2, 0, 0 }, { 2, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 16, 0, 0 }, { 16, 0, 0 }, {}, {} } },
                { { { 0, 16, 0 }, { 0, 16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 0, 0 }, { 0, 0, 0 }, {}, {} } },
                { { { 0, 2, 0 }, { 0, 2, 0 }, {}, {} } },
            } },
            { {
                { { { 0, 2, 0 }, { 0, 2, 0 }, { 0, 2, 0 }, { 0, 2, 0 } } },
                { { {}, {}, {}, {} } },
                { { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
                { { { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 0 } } },
                { { {}, {}, {}, {} } },
                { { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
                { { { 2, 0, 0 }, { 2, 0, 0 }, { 2, 0, 0 }, { 2, 0, 0 } } },
            } },
            { {
                { { { 2, 0, 0 }, { 2, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 0, 0 }, { 0, 0, 0 }, {}, {} } },
                { { { 16, 0, 0 }, { 16, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 16, 0 }, { 0, 16, 0 }, {}, {} } },
                { { { 0, 2, 0 }, { 0, 2, 0 }, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kRightQuarterTurn5TilesBoundBoxes = { {
            { {
                { { { { 0, 2, 0 }, { 32, 32, 2 } },
                    { { 0, 2, 0 }, { 32, 32, 2 } },
                    { { 0, 2, 27 }, { 32, 32, 0 } },
                    { { 0, 2, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 32, 32, 2 } },
                    { { 2, 0, 0 }, { 32, 32, 2 } },
                    { { 2, 0, 27 }, { 32, 32, 0 } },
                    { { 2, 0, 27 }, { 32, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 32, 32, 2 } }, { { 2, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 34, 2 } }, { { 16, 0, 0 }, { 16, 34, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 27 }, { 32, 27, 0 } },
                    { { 0, 2, 27 }, { 32, 27, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 27 }, { 32, 16, 0 } },
                    { { 0, 0, 27 }, { 32, 16, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 2, 0 }, { 32, 32, 2 } }, { { 0, 2, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        {{ 23497, 24363, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23498, 24364, 23525, 24391, }},
        {{ 23499, 24365, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23500, 24366, 23526, 24392, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kFlatToLeftBankBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        {{ 23501, 24367, 23536, 24402, }},
        {{ 23502, 24368, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23503, 24369, 23527, 24393, }},
        {{ 23504, 24370, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kBankedRightQuarterTurn5TilesSprites = {{
        {{
            {{ 23635, 24501, 23655, 24521, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23636, 24502, 23656, 24522, }},
            {{ 23637, 24503, 23657, 24523, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23638, 24504, 23658, 24524, }},
            {{ 23639, 24505, 23659, 24525, }},
        }},
        {{
            {{ 23640, 24506, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23641, 24507, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23642, 24508, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23643, 24509, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23644, 24510, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23645, 24511, 23660, 24526, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23646, 24512, 23661, 24527, }},
            {{ 23647, 24513, 23662, 24528, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23648, 24514, 23663, 24529, }},
            {{ 23649, 24515, 23664, 24530, }},
        }},
        {{
            {{ 23650, 24516, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23651, 24517, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23652, 24518, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23653, 24519, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23654, 24520, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kBankedRightQuarterTurn5TilesBoundBoxes = { {
            { {
                { { { { 0, 2, 0 }, { 32, 32, 2 } },
                    { { 0, 2, 0 }, { 32, 32, 2 } },
                    { { 0, 2, 27 }, { 32, 32, 0 } },
                    { { 0, 2, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 32, 32, 2 } },
                    { { 2, 0, 0 }, { 32, 32, 2 } },
                    { { 2, 0, 27 }, { 32, 32, 0 } },
                    { { 2, 0, 27 }, { 32, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 32, 32, 2 } }, { { 2, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 27 }, { 32, 27, 0 } },
                    { { 0, 2, 27 }, { 32, 27, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 27 }, { 32, 16, 0 } },
                    { { 0, 0, 27 }, { 32, 16, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 2, 0 }, { 32, 32, 2 } }, { { 0, 2, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        {{ 23517, 24383, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23518, 24384, 23532, 24398, }},
        {{ 23519, 24385, 23533, 24399, }},
        {{ 23520, 24386, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftBankToUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        {{ 23521, 24387, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23522, 24388, 23534, 24400, }},
        {{ 23523, 24389, 23535, 24401, }},
        {{ 23524, 24390, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 23509, 24375, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23510, 24376, 23528, 24394, }},
        {{ 23511, 24377, 23529, 24395, }},
        {{ 23512, 24378, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25ToLeftBankBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        {{ 23513, 24379, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23514, 24380, 23530, 24396, }},
        {{ 23515, 24381, 23531, 24397, }},
        {{ 23516, 24382, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        {{ 23505, 24371, SPR_G2_WOODEN_RC_LEFT_BANK_FRONT_SW_NE + 0, SPR_G2_WOODEN_RC_LEFT_BANK_RAILS_FRONT_SW_NE + 0, }},
        {{ 23506, 24372, SPR_G2_WOODEN_RC_LEFT_BANK_FRONT_NW_SE + 0, SPR_G2_WOODEN_RC_LEFT_BANK_RAILS_FRONT_NW_SE + 0, }},
        {{ 23507, 24373, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 23508, 24374, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftBankBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            {{ 23781, 24647, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23782, 24648, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23783, 24649, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23784, 24650, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23785, 24651, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23786, 24652, 23812, 24678, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23787, 24653, 23813, 24679, }},
            {{ 23788, 24654, 23814, 24680, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23789, 24655, 23815, 24681, }},
            {{ 23790, 24656, 23816, 24682, }},
        }},
        {{
            {{ 23791, 24657, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23792, 24658, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23793, 24659, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23794, 24660, 23817, 24683, }},
            {{ 23795, 24661, 23818, 24684, }},
        }},
        {{
            {{ 23796, 24662, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23797, 24663, 23819, 24685, }},
            {{ 23798, 24664, 23820, 24686, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23799, 24665, 23821, 24687, }},
            {{ 23800, 24666, 23822, 24688, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kLeftQuarterTurn5TilesUp25BoundBoxes = { {
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 59 }, { 16, 16, 0 } },
                    { { 16, 16, 59 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 67 }, { 32, 16, 0 } },
                    { { 0, 0, 67 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } } } },
            } },
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 67 }, { 16, 32, 0 } },
                    { { 16, 0, 67 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 59 }, { 16, 16, 0 } },
                    { { 0, 0, 59 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 67 }, { 32, 16, 0 } },
                    { { 0, 16, 67 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            {{ 23761, 24627, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23762, 24628, 23801, 24667, }},
            {{ 23763, 24629, 23802, 24668, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23764, 24630, 23803, 24669, }},
            {{ 23765, 24631, 23804, 24670, }},
        }},
        {{
            {{ 23766, 24632, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23767, 24633, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23768, 24634, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23769, 24635, 23805, 24671, }},
            {{ 23770, 24636, 23806, 24672, }},
        }},
        {{
            {{ 23771, 24637, 23807, 24673, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23772, 24638, 23808, 24674, }},
            {{ 23773, 24639, 23809, 24675, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23774, 24640, 23810, 24676, }},
            {{ 23775, 24641, 23811, 24677, }},
        }},
        {{
            {{ 23776, 24642, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23777, 24643, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23778, 24644, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23779, 24645, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23780, 24646, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            {{ 23725, 24591, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23726, 24592, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23727, 24593, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23728, 24594, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23729, 24595, 23741, 24607, }},
            {{ 23730, 24596, 23742, 24608, }},
            {{ 23731, 24597, 23743, 24609, }},
            {{ 23732, 24598, 23744, 24610, }},
        }},
        {{
            {{ 23728, 24594, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23727, 24593, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23726, 24592, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23725, 24591, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23732, 24598, 23744, 24610, }},
            {{ 23731, 24597, 23743, 24609, }},
            {{ 23730, 24596, 23742, 24608, }},
            {{ 23729, 24595, 23741, 24607, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kSBendLeftBoundBoxes = { {
        { {
            { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
            { { { { 0, 0, 0 }, { 32, 26, 2 } }, { { 0, 0, 0 }, { 32, 26, 2 } }, {}, {} } },
            { { { { 0, 6, 0 }, { 32, 26, 2 } }, { { 0, 6, 0 }, { 32, 26, 2 } }, {}, {} } },
            { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        } },
        { {
            { { { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 27 }, { 25, 32, 0 } },
                { { 3, 0, 27 }, { 25, 32, 0 } } } },
            { { { { 0, 0, 0 }, { 26, 32, 2 } },
                { { 0, 0, 0 }, { 26, 32, 2 } },
                { { 0, 0, 27 }, { 26, 32, 0 } },
                { { 0, 0, 27 }, { 26, 32, 0 } } } },
            { { { { 6, 0, 0 }, { 26, 32, 2 } },
                { { 6, 0, 0 }, { 26, 32, 2 } },
                { { 6, 0, 27 }, { 26, 32, 0 } },
                { { 6, 0, 27 }, { 26, 32, 0 } } } },
            { { { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 2, 0, 27 }, { 27, 32, 0 } },
                { { 2, 0, 27 }, { 27, 32, 0 } } } },
        } },
        { {
            { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
            { { { { 0, 6, 0 }, { 32, 26, 2 } }, { { 0, 6, 0 }, { 32, 26, 2 } }, {}, {} } },
            { { { { 0, 0, 0 }, { 32, 26, 2 } }, { { 0, 0, 0 }, { 32, 26, 2 } }, {}, {} } },
            { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        } },
        { {
            { { { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 2, 0, 27 }, { 27, 32, 0 } },
                { { 2, 0, 27 }, { 27, 32, 0 } } } },
            { { { { 6, 0, 0 }, { 26, 32, 2 } },
                { { 6, 0, 0 }, { 26, 32, 2 } },
                { { 6, 0, 27 }, { 26, 32, 0 } },
                { { 6, 0, 27 }, { 26, 32, 0 } } } },
            { { { { 0, 0, 0 }, { 26, 32, 2 } },
                { { 0, 0, 0 }, { 26, 32, 2 } },
                { { 0, 0, 27 }, { 26, 32, 0 } },
                { { 0, 0, 27 }, { 26, 32, 0 } } } },
            { { { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 27 }, { 25, 32, 0 } },
                { { 3, 0, 27 }, { 25, 32, 0 } } } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            {{ 23733, 24599, 23745, 24611, }},
            {{ 23734, 24600, 23746, 24612, }},
            {{ 23735, 24601, 23747, 24613, }},
            {{ 23736, 24602, 23748, 24614, }},
        }},
        {{
            {{ 23737, 24603, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23738, 24604, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23739, 24605, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23740, 24606, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23736, 24602, 23748, 24614, }},
            {{ 23735, 24601, 23747, 24613, }},
            {{ 23734, 24600, 23746, 24612, }},
            {{ 23733, 24599, 23745, 24611, }},
        }},
        {{
            {{ 23740, 24606, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23739, 24605, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23738, 24604, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23737, 24603, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            23453,
            23454,
            23455,
            23456,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23457,
            23458,
            23459,
            23460,
        }},
        {{
            23461,
            23462,
            23463,
            23464,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23465,
            23466,
            23467,
            23468,
        }},
        {{
            23460,
            23459,
            23458,
            23457,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23456,
            23455,
            23454,
            23453,
        }},
        {{
            23468,
            23467,
            23466,
            23465,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23464,
            23463,
            23462,
            23461,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections> kLeftVerticalLoopOffsets = { {
        { {
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
        } },
        { {
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
        } },
        { {
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
        } },
        { {
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
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections> kLeftVerticalLoopBoundBoxes = { {
        { {
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
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 14, 0, 0 }, { 2, 32, 63 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 0, 0, 32 }, { 16, 32, 3 } },
            {},
            {},
            { { 16, 0, 32 }, { 16, 32, 3 } },
            { { 16, 16, 0 }, { 16, 4, 119 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 7 } },
        } },
        { {
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
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 7 } },
            { { 6, 0, 0 }, { 26, 32, 3 } },
            { { 16, 16, 0 }, { 16, 4, 119 } },
            { { 16, 0, 32 }, { 16, 32, 3 } },
            {},
            {},
            { { 0, 0, 32 }, { 16, 32, 3 } },
            { { 0, 12, 0 }, { 16, 3, 119 } },
            { { 14, 0, 0 }, { 2, 32, 63 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopSprites = {{
        {{
            23484,
            23483,
            23482,
            23481,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23480,
            23479,
            23478,
            23477,
        }},
        {{
            23476,
            23475,
            23474,
            23473,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23472,
            23471,
            23470,
            23469,
        }},
        {{
            23477,
            23478,
            23479,
            23480,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23481,
            23482,
            23483,
            23484,
        }},
        {{
            23469,
            23470,
            23471,
            23472,
            kImageIndexUndefined,
            kImageIndexUndefined,
            23473,
            23474,
            23475,
            23476,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 10>, kNumOrthogonalDirections> kRightVerticalLoopOffsets = { {
        { {
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
        } },
        { {
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
        } },
        { {
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
        } },
        { {
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
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 10>, kNumOrthogonalDirections>
        kRightVerticalLoopBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftVerticalLoopBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            {{ 23828, 24694, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23827, 24693, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23826, 24692, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23831, 24697, 23840, 24706, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23830, 24696, 23839, 24705, }},
            {{ 23829, 24695, 23838, 24704, }},
        }},
        {{
            {{ 23834, 24700, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23833, 24699, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23832, 24698, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23825, 24691, 23837, 24703, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23824, 24690, 23836, 24702, }},
            {{ 23823, 24689, 23835, 24701, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftQuarterTurn3TilesOffsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 16, 0, 0 }, { 16, 0, 0 }, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 16, 0 }, { 0, 16, 0 }, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 0 }, { 16, 16, 0 } } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftQuarterTurn3TilesBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            {{ 23846, 24712, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23845, 24711, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23844, 24710, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23849, 24715, 23858, 24724, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23848, 24714, 23857, 24723, }},
            {{ 23847, 24713, 23856, 24722, }},
        }},
        {{
            {{ 23852, 24718, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23851, 24717, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23850, 24716, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23843, 24709, 23855, 24721, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23842, 24708, 23854, 24720, }},
            {{ 23841, 24707, 23853, 24719, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankedQuarterTurn3TilesBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ 23906, 24772, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23905, 24771, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23908, 24774, 23919, 24785, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23907, 24773, 23918, 24784, }},
        }},
        {{
            {{ 23910, 24776, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23909, 24775, 23920, 24786, }},
        }},
        {{
            {{ 23904, 24770, 23917, 24783, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23903, 24769, 23916, 24782, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftQuarterTurn3TilesUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftQuarterTurn3TilesUp25BoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ 23895, 24761, 23911, 24777, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23896, 24762, 23912, 24778, }},
        }},
        {{
            {{ 23897, 24763, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23898, 24764, 23913, 24779, }},
        }},
        {{
            {{ 23899, 24765, 23914, 24780, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23900, 24766, 23915, 24781, }},
        }},
        {{
            {{ 23901, 24767, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23902, 24768, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightQuarterTurn3TilesUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn3TilesUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 23882, 24748, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23881, 24747, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23880, 24746, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23879, 24745, 23891, 24757, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23878, 24744, 23890, 24756, }},
            {{ 23877, 24743, 23889, 24755, }},
        }},
        {{
            {{ 23885, 24751, 23894, 24760, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23884, 24750, 23893, 24759, }},
            {{ 23883, 24749, 23892, 24758, }},
            {{ 23882, 24748, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23881, 24747, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23880, 24746, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23888, 24754, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23887, 24753, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23886, 24752, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23885, 24751, 23894, 24760, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23884, 24750, 23893, 24759, }},
            {{ 23883, 24749, 23892, 24758, }},
        }},
        {{
            {{ 23879, 24745, 23891, 24757, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23878, 24744, 23890, 24756, }},
            {{ 23877, 24743, 23889, 24755, }},
            {{ 23888, 24754, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23887, 24753, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23886, 24752, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 8>, kNumOrthogonalDirections>
        kLeftHalfBankedHelixUpSmallBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 23859, 24725, 23871, 24737, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23860, 24726, 23872, 24738, }},
            {{ 23861, 24727, 23873, 24739, }},
            {{ 23862, 24728, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23863, 24729, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23864, 24730, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23862, 24728, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23863, 24729, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23864, 24730, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23865, 24731, 23874, 24740, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23866, 24732, 23875, 24741, }},
            {{ 23867, 24733, 23876, 24742, }},
        }},
        {{
            {{ 23865, 24731, 23874, 24740, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23866, 24732, 23875, 24741, }},
            {{ 23867, 24733, 23876, 24742, }},
            {{ 23868, 24734, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23869, 24735, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23870, 24736, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23868, 24734, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23869, 24735, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23870, 24736, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23859, 24725, 23871, 24737, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23860, 24726, 23872, 24738, }},
            {{ 23861, 24727, 23873, 24739, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 8>, kNumOrthogonalDirections>
        kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 23704, 24570, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23703, 24569, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23702, 24568, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23701, 24567, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23700, 24566, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23699, 24565, 23719, 24585, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23698, 24564, 23718, 24584, }},
            {{ 23697, 24563, 23717, 24583, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23696, 24562, 23716, 24582, }},
            {{ 23695, 24561, 23715, 24581, }},
        }},
        {{
            {{ 23709, 24575, 23724, 24590, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23708, 24574, 23723, 24589, }},
            {{ 23707, 24573, 23722, 24588, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23706, 24572, 23721, 24587, }},
            {{ 23705, 24571, 23720, 24586, }},
            {{ 23704, 24570, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23703, 24569, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23702, 24568, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23701, 24567, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23700, 24566, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23714, 24580, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23713, 24579, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23712, 24578, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23711, 24577, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23710, 24576, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23709, 24575, 23724, 24590, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23708, 24574, 23723, 24589, }},
            {{ 23707, 24573, 23722, 24588, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23706, 24572, 23721, 24587, }},
            {{ 23705, 24571, 23720, 24586, }},
        }},
        {{
            {{ 23699, 24565, 23719, 24585, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23698, 24564, 23718, 24584, }},
            {{ 23697, 24563, 23717, 24583, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23696, 24562, 23716, 24582, }},
            {{ 23695, 24561, 23715, 24581, }},
            {{ 23714, 24580, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23713, 24579, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23712, 24578, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23711, 24577, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23710, 24576, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 14>, kNumOrthogonalDirections>
        kLeftHalfBankedHelixUpLargeBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 23665, 24531, 23685, 24551, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23666, 24532, 23686, 24552, }},
            {{ 23667, 24533, 23687, 24553, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23668, 24534, 23688, 24554, }},
            {{ 23669, 24535, 23689, 24555, }},
            {{ 23670, 24536, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23671, 24537, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23672, 24538, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23673, 24539, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23674, 24540, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23670, 24536, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23671, 24537, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23672, 24538, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23673, 24539, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23674, 24540, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23675, 24541, 23690, 24556, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23676, 24542, 23691, 24557, }},
            {{ 23677, 24543, 23692, 24558, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23678, 24544, 23693, 24559, }},
            {{ 23679, 24545, 23694, 24560, }},
        }},
        {{
            {{ 23675, 24541, 23690, 24556, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23676, 24542, 23691, 24557, }},
            {{ 23677, 24543, 23692, 24558, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23678, 24544, 23693, 24559, }},
            {{ 23679, 24545, 23694, 24560, }},
            {{ 23680, 24546, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23681, 24547, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23682, 24548, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23683, 24549, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23684, 24550, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23680, 24546, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23681, 24547, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23682, 24548, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23683, 24549, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23684, 24550, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23665, 24531, 23685, 24551, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23666, 24532, 23686, 24552, }},
            {{ 23667, 24533, 23687, 24553, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23668, 24534, 23688, 24554, }},
            {{ 23669, 24535, 23689, 24555, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 14>, kNumOrthogonalDirections>
        kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ 24209, 25075, 24217, 25083, }},
        {{ 24210, 25076, 24218, 25084, }},
        {{ 24211, 25077, 24219, 25085, }},
        {{ 24212, 25078, 24220, 25086, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftQuarterTurn1TileUp60BoundBoxes = { {
        { { { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 2, 2, 86 }, { 28, 28, 1 } },
            { { 2, 2, 86 }, { 28, 28, 1 } } } },
        { { { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 26, 26, 0 }, { 2, 2, 87 } },
            { { 26, 26, 0 }, { 2, 2, 87 } } } },
        { { { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 26, 28, 0 }, { 2, 2, 87 } },
            { { 26, 28, 0 }, { 2, 2, 87 } } } },
        { { { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 2, 2, 0 }, { 28, 28, 3 } },
            { { 28, 28, 0 }, { 2, 2, 87 } },
            { { 28, 28, 0 }, { 2, 2, 87 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60Sprites = {{
        {{ 24213, 25079, 24221, 25087, }},
        {{ 24214, 25080, 24222, 25088, }},
        {{ 24215, 25081, 24223, 25089, }},
        {{ 24216, 25082, 24224, 25090, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBrakesSprites = {{
        {{ 23755, 24621, }},
        {{ 23756, 24622, }},
        {{ 23755, 24621, }},
        {{ 23756, 24622, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections> kBrakesOffsets = { {
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kBrakesBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBoosterSprites = {{
        {{ SPR_G2_WOODEN_RC_BOOSTER_SW_NE + 0, 24615, }},
        {{ SPR_G2_WOODEN_RC_BOOSTER_NW_SE + 0, 24616, }},
        {{ SPR_G2_WOODEN_RC_BOOSTER_SW_NE + 0, 24617, }},
        {{ SPR_G2_WOODEN_RC_BOOSTER_NW_SE + 0, 24618, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections> kBoosterOffsets = { {
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kBoosterBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25LeftBankedSprites = {{
        {{ 24249, 25115, SPR_G2_WOODEN_RC_24249_FRONT + 0, SPR_G2_WOODEN_RC_25115_FRONT + 0, }},
        {{ 24250, 25116, 24257, 25123, }},
        {{ 24251, 25117, 24258, 25124, }},
        {{ 24252, 25118, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25LeftBankedBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 4, 26, 5 }, { 24, 1, 9 } },
            { { 4, 26, 5 }, { 24, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25RightBankedSprites = {{
        {{ 24253, 25119, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24254, 25120, 24259, 25125, }},
        {{ 24255, 25121, 24260, 25126, }},
        {{ 24256, 25122, SPR_G2_WOODEN_RC_24256_FRONT + 0, SPR_G2_WOODEN_RC_25122_FRONT + 0, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kWatersplashSprites = {{
        {{
            {{ 23989, 24855, 23993, 24859, }},
            {{ 23977, 24843, 23981, 24847, }},
            {{ 23999, 24865, 24001, 24867, }},
            {{ 23975, 24841, 23979, 24845, }},
            {{ 23987, 24853, 23991, 24857, }},
        }},
        {{
            {{ 23990, 24856, 23994, 24860, }},
            {{ 23978, 24844, 23982, 24848, }},
            {{ 24000, 24866, 24002, 24868, }},
            {{ 23976, 24842, 23980, 24846, }},
            {{ 23988, 24854, 23992, 24858, }},
        }},
        {{
            {{ 23987, 24853, 23991, 24857, }},
            {{ 23975, 24841, 23979, 24845, }},
            {{ 23999, 24865, 24001, 24867, }},
            {{ 23977, 24843, 23981, 24847, }},
            {{ 23989, 24855, 23993, 24859, }},
        }},
        {{
            {{ 23988, 24854, 23992, 24858, }},
            {{ 23976, 24842, 23980, 24846, }},
            {{ 24000, 24866, 24002, 24868, }},
            {{ 23978, 24844, 23982, 24848, }},
            {{ 23990, 24856, 23994, 24860, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections> kWatersplashBoundBoxes = {
        {
            { {
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
            } },
            { {
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
            } },
            { {
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
                { { { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } },
                    { { 0, 3, 0 }, { 32, 25, 2 } } } },
            } },
            { {
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
                { { { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } },
                    { { 3, 0, 0 }, { 25, 32, 2 } } } },
            } },
        }
    };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 0, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 0, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 1, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 1, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 2, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 2, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 3, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 3, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 4, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 4, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 0, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 0, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 5, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 5, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 1, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 1, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 6, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 6, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 2, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 2, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 7, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 7, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 8, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 8, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 4, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 4, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 9, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 9, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 5, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 5, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 10, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 10, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 6, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 6, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 11, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 11, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 12, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 12, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 13, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 13, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 14, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 14, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 15, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 15, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kFlatToUp60LongBaseBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 16 }, { 18, 32, 0 } },
                    { { 6, 0, 16 }, { 18, 32, 0 } } } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 32 }, { 18, 32, 0 } },
                    { { 6, 0, 32 }, { 18, 32, 0 } } } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 35 }, { 18, 32, 0 } },
                    { { 6, 0, 35 }, { 18, 32, 0 } } } },
                { { { { 4, 28, -16 }, { 24, 2, 56 } }, { { 4, 28, -16 }, { 24, 2, 56 } }, {}, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 16 }, { 32, 18, 0 } },
                    { { 0, 6, 16 }, { 32, 18, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 32 }, { 32, 18, 0 } },
                    { { 0, 6, 32 }, { 32, 18, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 35 }, { 32, 18, 0 } },
                    { { 0, 6, 35 }, { 32, 18, 0 } } } },
                { { { { 28, 4, -16 }, { 2, 24, 56 } }, { { 28, 4, -16 }, { 2, 24, 56 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseSprites = {{
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 16, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 16, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 17, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 17, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 18, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 18, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 19, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 19, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 20, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 20, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 21, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 21, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 9, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 9, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 22, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 22, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 10, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 10, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 23, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 23, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 11, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 11, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 24, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 24, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 25, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 25, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 13, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 13, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 26, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 26, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 14, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 14, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 27, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 27, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT + 15, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_FRONT_RAILS + 15, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 28, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 28, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 29, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 29, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 30, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 30, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_FLAT_TO_STEEP + 31, SPR_G2_WOODEN_RC_FLAT_TO_STEEP_RAILS + 31, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kUp60ToFlatLongBaseBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 0 }, { 32, 20, 3 } }, {}, {} } },
            } },
            { {
                { { { { 4, 28, -16 }, { 24, 2, 76 } }, { { 4, 28, -16 }, { 24, 2, 76 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 40 }, { 18, 32, 0 } },
                    { { 6, 0, 40 }, { 18, 32, 0 } } } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 25 }, { 18, 32, 3 } },
                    { { 6, 0, 25 }, { 18, 32, 3 } } } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 0 }, { 20, 32, 3 } },
                    { { 6, 0, 25 }, { 18, 32, 0 } },
                    { { 6, 0, 25 }, { 18, 32, 0 } } } },
            } },
            { {
                { { { { 28, 4, -16 }, { 2, 24, 76 } }, { { 28, 4, -16 }, { 2, 24, 76 } }, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 40 }, { 32, 18, 0 } },
                    { { 0, 6, 40 }, { 32, 18, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 25 }, { 32, 18, 3 } },
                    { { 0, 6, 25 }, { 32, 18, 3 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 0 }, { 32, 20, 3 } },
                    { { 0, 6, 25 }, { 32, 18, 0 } },
                    { { 0, 6, 25 }, { 32, 18, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 0 }, { 20, 32, 3 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            {{ 24137, 25003, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24138, 25004, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24139, 25005, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24140, 25006, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24141, 25007, 24153, 25019, }},
            {{ 24142, 25008, 24154, 25020, }},
            {{ 24143, 25009, 24155, 25021, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24144, 25010, 24156, 25022, }},
        }},
        {{
            {{ 24145, 25011, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24146, 25012, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24147, 25013, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24148, 25014, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24149, 25015, 24157, 25023, }},
            {{ 24150, 25016, 24158, 25024, }},
            {{ 24151, 25017, 24159, 25025, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24152, 25018, 24160, 25026, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections>
        kLeftEighthToDiagBoundBoxes = { {
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 34, 2 } },
                    { { 0, 0, 0 }, { 16, 34, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 18, 16, 2 } },
                    { { 16, 0, 0 }, { 18, 16, 2 } },
                    { { 16, 0, 27 }, { 16, 16, 0 } },
                    { { 16, 0, 27 }, { 16, 16, 0 } } } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } } } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } },
                    { { 0, 16, 0 }, { 16, 16, 2 } },
                    { { 0, 16, 27 }, { 16, 16, 0 } },
                    { { 0, 16, 27 }, { 16, 16, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            {{ 24113, 24979, 24129, 24995, }},
            {{ 24114, 24980, 24130, 24996, }},
            {{ 24115, 24981, 24131, 24997, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24116, 24982, 24132, 24998, }},
        }},
        {{
            {{ 24117, 24983, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24118, 24984, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24119, 24985, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24120, 24986, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24121, 24987, 24133, 24999, }},
            {{ 24122, 24988, 24134, 25000, }},
            {{ 24123, 24989, 24135, 25001, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24124, 24990, 24136, 25002, }},
        }},
        {{
            {{ 24125, 24991, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24126, 24992, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24127, 24993, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24128, 24994, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections>
        kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            {{ 24185, 25051, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24186, 25052, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24187, 25053, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24188, 25054, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24189, 25055, 24201, 25067, }},
            {{ 24190, 25056, 24202, 25068, }},
            {{ 24191, 25057, 24203, 25069, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24192, 25058, 24204, 25070, }},
        }},
        {{
            {{ 24193, 25059, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24194, 25060, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24195, 25061, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24196, 25062, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24197, 25063, 24205, 25071, }},
            {{ 24198, 25064, 24206, 25072, }},
            {{ 24199, 25065, 24207, 25073, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24200, 25066, 24208, 25074, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections>
        kLeftEighthBankToDiagBoundBoxes = { {
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 34, 2 } },
                    { { 0, 0, 0 }, { 16, 34, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 18, 16, 2 } },
                    { { 16, 0, 0 }, { 18, 16, 2 } },
                    { { 16, 0, 27 }, { 16, 16, 0 } },
                    { { 16, 0, 27 }, { 16, 16, 0 } } } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } } } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } },
                    { { 0, 16, 0 }, { 16, 16, 2 } },
                    { { 0, 16, 27 }, { 16, 16, 0 } },
                    { { 0, 16, 27 }, { 16, 16, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            {{ 24161, 25027, 24177, 25043, }},
            {{ 24162, 25028, 24178, 25044, }},
            {{ 24163, 25029, 24179, 25045, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24164, 25030, 24180, 25046, }},
        }},
        {{
            {{ 24165, 25031, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24166, 25032, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24167, 25033, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24168, 25034, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24169, 25035, 24181, 25047, }},
            {{ 24170, 25036, 24182, 25048, }},
            {{ 24171, 25037, 24183, 25049, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24172, 25038, 24184, 25050, }},
        }},
        {{
            {{ 24173, 25039, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24174, 25040, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24175, 25041, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24176, 25042, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections>
        kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24005, 24871, 24009, 24875, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24006, 24872, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24007, 24873, 24010, 24876, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24008, 24874, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24047, 24913, 24051, 24917, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24048, 24914, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24049, 24915, 24052, 24918, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24050, 24916, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagFlatOffsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagFlatBoundBoxes = { {
        { {
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 24 }, { 32, 32, 0 } },
                { { -16, -16, 24 }, { 32, 32, 0 } } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 24 }, { 32, 32, 0 } },
                { { -16, -16, 24 }, { 32, 32, 0 } } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25Sprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24023, 24889, 24027, 24893, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24024, 24890, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24025, 24891, 24028, 24894, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24026, 24892, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24065, 24931, 24069, 24935, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24066, 24932, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24067, 24933, 24070, 24936, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24068, 24934, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp25Offsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp25BoundBoxes = { {
        { {
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 43 }, { 32, 32, 0 } },
                { { -16, -16, 43 }, { 32, 32, 0 } } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 43 }, { 32, 32, 0 } },
                { { -16, -16, 43 }, { 32, 32, 0 } } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24041, 24907, 24045, 24911, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24042, 24908, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24043, 24909, 24046, 24912, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24044, 24910, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp60Offsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp60BoundBoxes = { {
        { {
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 91 }, { 32, 32, 0 } },
                { { -16, -16, 91 }, { 32, 32, 0 } } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 91 }, { 32, 32, 0 } },
                { { -16, -16, 91 }, { 32, 32, 0 } } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatToUp25Sprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24011, 24877, 24015, 24881, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24012, 24878, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24013, 24879, 24016, 24882, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24014, 24880, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24053, 24919, 24057, 24923, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24054, 24920, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24055, 24921, 24058, 24924, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24056, 24922, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagFlatToUp25Offsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToUp25BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24029, 24895, 24033, 24899, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24030, 24896, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24031, 24897, 24034, 24900, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24032, 24898, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp25ToUp60Offsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToUp60BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24035, 24901, 24039, 24905, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24036, 24902, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24037, 24903, 24040, 24906, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24038, 24904, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp60ToUp25Offsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp60ToUp25BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } },
                    { { -16, -16, 59 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25ToFlatSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24017, 24883, 24021, 24887, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24018, 24884, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24019, 24885, 24022, 24888, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24020, 24886, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24059, 24925, 24063, 24929, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24060, 24926, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 24061, 24927, 24064, 24930, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 24062, 24928, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagUp25ToFlatOffsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToFlatBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24077, 24943, 24081, 24947, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24078, 24944, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24079, 24945, 24082, 24948, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24080, 24946, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToLeftBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToLeftBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24083, 24949, 24087, 24953, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24084, 24950, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24085, 24951, 24088, 24954, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24086, 24952, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToRightBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToRightBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24101, 24967, 24105, 24971, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24102, 24968, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24103, 24969, 24106, 24972, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24104, 24970, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagLeftBankToUp25Offsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagLeftBankToUp25BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24107, 24973, 24111, 24977, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24108, 24974, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24109, 24975, 24112, 24978, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24110, 24976, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagRightBankToUp25Offsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagRightBankToUp25BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24089, 24955, 24093, 24959, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24090, 24956, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24091, 24957, 24094, 24960, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24092, 24958, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToLeftBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToLeftBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24095, 24961, 24099, 24965, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24096, 24962, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24097, 24963, 24100, 24966, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24098, 24964, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToRightBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToRightBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24071, 24937, 24075, 24941, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24072, 24938, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24073, 24939, 24076, 24942, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24074, 24940, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagLeftBankOffsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagLeftBankBoundBoxes = {
        {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        }
    };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ 23958, 24824, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23957, 24823, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23960, 24826, 23971, 24837, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23959, 24825, 23970, 24836, }},
        }},
        {{
            {{ 23962, 24828, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23961, 24827, 23972, 24838, }},
        }},
        {{
            {{ 23956, 24822, 23969, 24835, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23955, 24821, 23968, 24834, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankToLeftQuarterTurn3TilesUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ 23947, 24813, 23963, 24829, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23948, 24814, 23964, 24830, }},
        }},
        {{
            {{ 23949, 24815, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23950, 24816, 23965, 24831, }},
        }},
        {{
            {{ 23951, 24817, 23966, 24832, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23952, 24818, 23967, 24833, }},
        }},
        {{
            {{ 23953, 24819, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23954, 24820, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightBankToRightQuarterTurn3TilesUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightBankToRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
            kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 3, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 4, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 5, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 56, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 9, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 57, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 10, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 11, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 12, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 13, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 14, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 15, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 58, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 17, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 18, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 19, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 20, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 21, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 22, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 23, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 24, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 25, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 26, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 59, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 27, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
        kLeftLargeHalfLoopUpBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 3 } }, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 3 } }, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 3 } }, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 3 } }, { { 0, 16, 210 }, { 32, 16, 3 } } } },
                { { { { 0, 16, 32 }, { 32, 16, 0 } }, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 9 } }, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 0 } }, { { 0, 0, 70 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 200 }, { 16, 32, 0 } }, {} } },
                { { { { 16, 0, 110 }, { 16, 16, 0 } }, {} } },
                { { { { 16, 0, 200 }, { 16, 32, 0 } }, {} } },
                { { { { 16, 0, 32 }, { 16, 32, 0 } }, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 64 }, { 32, 20, 3 } } } },
                { { { { 0, 16, 80 }, { 32, 16, 0 } }, {} } },
                { { { { 0, 16, 200 }, { 32, 16, 0 } }, {} } },
                { { { { 0, 0, 100 }, { 16, 16, 0 } }, {} } },
                { { { { 0, 0, 200 }, { 32, 16, 0 } }, {} } },
                { { { { 0, 0, 32 }, { 32, 16, 0 } }, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 3 } }, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 3 } }, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 3 } }, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 3 } }, { { 0, 0, 210 }, { 16, 32, 3 } } } },
                { { { { 0, 0, 32 }, { 16, 32, 0 } }, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 28, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 29, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 30, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 31, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 33, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 60, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 35, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 36, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 61, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 37, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 38, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 39, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 40, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 41, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 42, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 43, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 44, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 62, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 45, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 46, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 47, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 48, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 49, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 50, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 51, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 52, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 53, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 54, SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 63, }},
            {{ SPR_G2_WOODEN_RC_LARGE_HALF_LOOP + 55, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
        kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            {{ 23757, 24623, }},
            {{ 23758, 24624, }},
            {{ 23757, 24623, }},
            {{ 23758, 24624, }},
        }},
        {{
            {{ 23759, 24623, }},
            {{ 23760, 24624, }},
            {{ 23759, 24623, }},
            {{ 23760, 24624, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections> kBlockBrakesOffsets = { {
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
        { { { 0, 2, 0 }, { 0, 2, 0 } } },
        { { { 2, 0, 0 }, { 2, 0, 0 } } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kBlockBrakesBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Sprites = {{
        {{
            {{ 23932, 24798, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23931, 24797, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 23934, 24800, 23945, 24811, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23933, 24799, 23944, 24810, }},
        }},
        {{
            {{ 23936, 24802, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23935, 24801, 23946, 24812, }},
        }},
        {{
            {{ 23930, 24796, 23943, 24809, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23929, 24795, 23942, 24808, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankedQuarterTurn3TileUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankedQuarterTurn3TileUp25BoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TileUp25Sprites = {{
        {{
            {{ 23921, 24787, 23937, 24803, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23922, 24788, 23938, 24804, }},
        }},
        {{
            {{ 23923, 24789, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23924, 24790, 23939, 24805, }},
        }},
        {{
            {{ 23925, 24791, 23940, 24806, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23926, 24792, 23941, 24807, }},
        }},
        {{
            {{ 23927, 24793, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 23928, 24794, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightBankedQuarterTurn3TileUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightBankedQuarterTurn3TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
            kLeftBankedQuarterTurn3TileUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn5TileUp25Sprites = {{
        {{
            {{ 24321, 25187, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24322, 25188, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24323, 25189, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24324, 25190, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24325, 25191, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 24326, 25192, 24352, 25218, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24327, 25193, 24353, 25219, }},
            {{ 24328, 25194, 24354, 25220, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24329, 25195, 24355, 25221, }},
            {{ 24330, 25196, 24356, 25222, }},
        }},
        {{
            {{ 24331, 25197, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24332, 25198, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24333, 25199, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24334, 25200, 24357, 25223, }},
            {{ 24335, 25201, 24358, 25224, }},
        }},
        {{
            {{ 24336, 25202, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24337, 25203, 24359, 25225, }},
            {{ 24338, 25204, 24360, 25226, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24339, 25205, 24361, 25227, }},
            {{ 24340, 25206, 24362, 25228, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kLeftBankedQuarterTurn5TileUp25BoundBoxes = { {
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 59 }, { 16, 16, 0 } },
                    { { 16, 16, 59 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 67 }, { 32, 16, 0 } },
                    { { 0, 0, 67 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } } } },
            } },
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } },
                    { { 0, 0, 67 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } },
                    { { 2, 0, 67 }, { 27, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 67 }, { 16, 32, 0 } },
                    { { 16, 0, 67 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 59 }, { 16, 16, 0 } },
                    { { 0, 0, 59 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 67 }, { 32, 16, 0 } },
                    { { 0, 16, 67 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } },
                    { { 0, 2, 67 }, { 32, 27, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25Sprites = {{
        {{
            {{ 24301, 25167, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24302, 25168, 24341, 25207, }},
            {{ 24303, 25169, 24342, 25208, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24304, 25170, 24343, 25209, }},
            {{ 24305, 25171, 24344, 25210, }},
        }},
        {{
            {{ 24306, 25172, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24307, 25173, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24308, 25174, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24309, 25175, 24345, 25211, }},
            {{ 24310, 25176, 24346, 25212, }},
        }},
        {{
            {{ 24311, 25177, 24347, 25213, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24312, 25178, 24348, 25214, }},
            {{ 24313, 25179, 24349, 25215, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24314, 25180, 24350, 25216, }},
            {{ 24315, 25181, 24351, 25217, }},
        }},
        {{
            {{ 24316, 25182, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24317, 25183, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24318, 25184, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24319, 25185, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 24320, 25186, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
            kLeftBankedQuarterTurn5TileUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ 24261, 25127, SPR_G2_WOODEN_RC_24261_FRONT + 0, SPR_G2_WOODEN_RC_25127_FRONT + 0, }},
        {{ 24262, 25128, SPR_G2_WOODEN_RC_24262_FRONT + 0, SPR_G2_WOODEN_RC_25128_FRONT + 0, }},
        {{ 24263, 25129, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24264, 25130, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25ToLeftBankedUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25Sprites = {{
        {{ 24265, 25131, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24266, 25132, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24267, 25133, SPR_G2_WOODEN_RC_24267_FRONT + 0, SPR_G2_WOODEN_RC_25133_FRONT + 0, }},
        {{ 24268, 25134, SPR_G2_WOODEN_RC_24268_FRONT + 0, SPR_G2_WOODEN_RC_25134_FRONT + 0, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ 24269, 25135, }},
        {{ 24270, 25136, }},
        {{ 24271, 25137, }},
        {{ 24272, 25138, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kLeftBankedUp25ToUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25Sprites = {{
        {{ 24273, 25139, }},
        {{ 24274, 25140, }},
        {{ 24275, 25141, }},
        {{ 24276, 25142, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
        kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        {{ 24277, 25143, SPR_G2_WOODEN_RC_24277_FRONT + 0, SPR_G2_WOODEN_RC_25143_FRONT + 0, }},
        {{ 24278, 25144, 24293, 25159, }},
        {{ 24279, 25145, 24294, 25160, }},
        {{ 24280, 25146, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kLeftBankedFlatToLeftBankedUp25BoundBoxes = { {
            { { { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 26, 5 }, { 32, 1, 9 } },
                { { 0, 26, 5 }, { 32, 1, 9 } } } },
            { { { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 26, 0, 5 }, { 1, 32, 9 } },
                { { 26, 0, 5 }, { 1, 32, 9 } } } },
            { { { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 26, 5 }, { 32, 1, 9 } },
                { { 0, 26, 5 }, { 32, 1, 9 } } } },
            { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25Sprites = {{
        {{ 24281, 25147, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24282, 25148, 24295, 25161, }},
        {{ 24283, 25149, 24296, 25162, }},
        {{ 24284, 25150, SPR_G2_WOODEN_RC_24284_FRONT + 0, SPR_G2_WOODEN_RC_25150_FRONT + 0, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
            kLeftBankedFlatToLeftBankedUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        {{ 24285, 25151, SPR_G2_WOODEN_RC_24285_FRONT + 0, SPR_G2_WOODEN_RC_25151_FRONT + 0, }},
        {{ 24286, 25152, 24297, 25163, }},
        {{ 24287, 25153, 24298, 25164, }},
        {{ 24288, 25154, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kLeftBankedUp25ToLeftBankedFlatBoundBoxes = { {
            { { { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 26, 5 }, { 32, 1, 9 } },
                { { 0, 26, 5 }, { 32, 1, 9 } } } },
            { { { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 3, 0, 0 }, { 25, 32, 2 } },
                { { 26, 0, 5 }, { 1, 32, 9 } },
                { { 26, 0, 5 }, { 1, 32, 9 } } } },
            { { { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 3, 0 }, { 32, 25, 2 } },
                { { 0, 26, 5 }, { 32, 1, 9 } },
                { { 0, 26, 5 }, { 32, 1, 9 } } } },
            { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatSprites = {{
        {{ 24289, 25155, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24290, 25156, 24299, 25165, }},
        {{ 24291, 25157, 24300, 25166, }},
        {{ 24292, 25158, SPR_G2_WOODEN_RC_24292_FRONT + 0, SPR_G2_WOODEN_RC_25158_FRONT + 0, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(
            kLeftBankedUp25ToLeftBankedFlatBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ 24225, 25091, SPR_G2_WOODEN_RC_24225_FRONT + 0, SPR_G2_WOODEN_RC_25091_FRONT + 0, }},
        {{ 24226, 25092, 24241, 25107, }},
        {{ 24227, 25093, 24242, 25108, }},
        {{ 24228, 25094, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kFlatToLeftBankedUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25Sprites = {{
        {{ 24229, 25095, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24230, 25096, 24243, 25109, }},
        {{ 24231, 25097, 24244, 25110, }},
        {{ 24232, 25098, SPR_G2_WOODEN_RC_24232_FRONT + 0, SPR_G2_WOODEN_RC_25098_FRONT + 0, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ 24233, 25099, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24234, 25100, 24245, 25111, }},
        {{ 24235, 25101, 24246, 25112, }},
        {{ 24236, 25102, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftBankedUp25ToFlatBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatSprites = {{
        {{ 24237, 25103, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ 24238, 25104, 24247, 25113, }},
        {{ 24239, 25105, 24248, 25114, }},
        {{ 24240, 25106, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 0, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 1, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 2, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 3, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 44, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 5, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 40, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 6, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 41, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 7, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 8, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 9, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 10, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 45, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 11, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 12, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 13, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 14, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 15, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 16, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 17, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 18, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 19, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
        kLeftMediumHalfLoopUpBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, {} } },
                { { { { 0, 0, 2 }, { 32, 32, 0 } }, {} } },
                { { { { 2, 0, 0 }, { 1, 32, 160 } }, { { 0, 0, 144 }, { 32, 20, 0 } } } },
                { { { { 0, 2, 48 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, { { 0, 0, 44 }, { 20, 32, 2 } } } },
                { { { { 0, 0, 0 }, { 20, 32, 3 } }, { { 0, 0, 70 }, { 20, 32, 2 } } } },
                { { { { 0, 29, 0 }, { 32, 1, 96 } }, {} } },
                { { { { 0, 0, 140 }, { 32, 32, 3 } }, {} } },
                { { { { 2, 0, 48 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 44 }, { 32, 20, 3 } } } },
                { { { { 0, 32, 0 }, { 40, 0, 64 } }, {} } },
                { { { { 31, 0, 0 }, { 1, 32, 96 } }, {} } },
                { { { { 29, 0, 0 }, { 1, 32, 160 } }, {} } },
                { { { { 0, 0, 48 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 3 } }, {} } },
                { { { { 0, 0, 0 }, { 32, 32, 0 } }, {} } },
                { { { { 0, 0, 0 }, { 32, 1, 160 } }, {} } },
                { { { { 0, 0, 48 }, { 32, 32, 1 } }, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpSprites = {{
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 20, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 21, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 22, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 23, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 25, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 46, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 26, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 27, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 28, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 29, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 30, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 42, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 31, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 43, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 32, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 33, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 35, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 36, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 37, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 38, SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 47, }},
            {{ SPR_G2_WOODEN_RC_MEDIUM_HALF_LOOP + 39, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
        kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 0, 24913, SPR_G2_WOODEN_RC_DIAG_BRAKES + 0, 24913, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 1, 24916, SPR_G2_WOODEN_RC_DIAG_BRAKES + 1, 24916, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 0, 24913, SPR_G2_WOODEN_RC_DIAG_BRAKES + 0, 24913, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 1, 24916, SPR_G2_WOODEN_RC_DIAG_BRAKES + 1, 24916, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagBrakesOffsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagBrakesBoundBoxes = { {
        { {
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } } } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } },
                { { -16, -16, 0 }, { 32, 32, 2 } } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>, 2>
    kDiagBlockBrakesSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 3, 24913, SPR_G2_WOODEN_RC_DIAG_BRAKES + 3, 24913, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 5, 24916, SPR_G2_WOODEN_RC_DIAG_BRAKES + 5, 24916, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 3, 24913, SPR_G2_WOODEN_RC_DIAG_BRAKES + 3, 24913, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 5, 24916, SPR_G2_WOODEN_RC_DIAG_BRAKES + 5, 24916, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 2, 24913, SPR_G2_WOODEN_RC_DIAG_BRAKES + 2, 24913, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 4, 24916, SPR_G2_WOODEN_RC_DIAG_BRAKES + 4, 24916, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 2, 24913, SPR_G2_WOODEN_RC_DIAG_BRAKES + 2, 24913, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ SPR_G2_WOODEN_RC_DIAG_BRAKES + 4, 24916, SPR_G2_WOODEN_RC_DIAG_BRAKES + 4, 24916, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagBlockBrakesOffsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagBlockBrakesBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } } } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0][0], &kFlatOffsets[0][0], &kFlatBoundBoxes[0][0], false },             // Flat
        { &kEndStationSprites[0][0][0], &kEndStationOffsets[0], &kEndStationBoundBoxes[0], false }, // EndStation
        { &kStationSprites[0][0], &kStationOffsets[0], &kStationBoundBoxes[0], false },             // BeginStation
        { &kStationSprites[0][0], &kStationOffsets[0], &kStationBoundBoxes[0], false },             // MiddleStation
        { &kUp25Sprites[0][0][0], nullptr, &kUp25BoundBoxes[0][0], false },                         // Up25
        { &kUp60Sprites[0][0][0], nullptr, &kUp60BoundBoxes[0][0], false },                         // Up60
        { &kFlatToUp25Sprites[0][0][0], nullptr, &kFlatToUp25BoundBoxes[0][0], false },             // FlatToUp25
        { &kUp25ToUp60Sprites[0][0][0], nullptr, &kUp25ToUp60BoundBoxes[0][0], false },             // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0][0], nullptr, &kUp60ToUp25BoundBoxes[0][0], false },             // Up60ToUp25
        { &kUp25ToFlatSprites[0][0][0], nullptr, &kUp25ToFlatBoundBoxes[0][0], false },             // Up25ToFlat
        { nullptr, nullptr, nullptr, true },                                                        // Down25
        { nullptr, nullptr, nullptr, true },                                                        // Down60
        { nullptr, nullptr, nullptr, true },                                                        // FlatToDown25
        { nullptr, nullptr, nullptr, true },                                                        // Down25ToDown60
        { nullptr, nullptr, nullptr, true },                                                        // Down60ToDown25
        { nullptr, nullptr, nullptr, true },                                                        // Down25ToFlat
        { nullptr, nullptr, nullptr, true },                                                        // LeftQuarterTurn5Tiles
        { &kRightQuarterTurn5TilesSprites[0][0][0], &kRightQuarterTurn5TilesOffsets[0][0][0],
          &kRightQuarterTurn5TilesBoundBoxes[0][0][0], false },                                // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0][0], nullptr, &kFlatToLeftBankBoundBoxes[0][0], false },   // FlatToLeftBank
        { &kFlatToRightBankSprites[0][0], nullptr, &kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true },                                                   // LeftBankToFlat
        { nullptr, nullptr, nullptr, true },                                                   // RightBankToFlat
        { nullptr, nullptr, nullptr, true },                                                   // BankedLeftQuarterTurn5Tiles
        { &kBankedRightQuarterTurn5TilesSprites[0][0][0], nullptr, &kBankedRightQuarterTurn5TilesBoundBoxes[0][0][0],
          false },                                                                             // BankedRightQuarterTurn5Tiles
        { &kLeftBankToUp25Sprites[0][0], nullptr, &kLeftBankToUp25BoundBoxes[0][0], false },   // LeftBankToUp25
        { &kRightBankToUp25Sprites[0][0], nullptr, &kRightBankToUp25BoundBoxes[0][0], false }, // RightBankToUp25
        { &kUp25ToLeftBankSprites[0][0], nullptr, &kUp25ToLeftBankBoundBoxes[0][0], false },   // Up25ToLeftBank
        { &kUp25ToRightBankSprites[0][0], nullptr, &kUp25ToRightBankBoundBoxes[0][0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true },                                                   // LeftBankToDown25
        { nullptr, nullptr, nullptr, true },                                                   // RightBankToDown25
        { nullptr, nullptr, nullptr, true },                                                   // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true },                                                   // Down25ToRightBank
        { &kLeftBankSprites[0][0], nullptr, &kLeftBankBoundBoxes[0][0], false },               // LeftBank
        { nullptr, nullptr, nullptr, true },                                                   // RightBank
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0][0], nullptr, &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0][0],
          false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0][0], nullptr, &kRightQuarterTurn5TilesUp25BoundBoxes[0][0][0],
          false },                                                                         // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true },                                               // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true },                                               // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0][0], nullptr, &kSBendLeftBoundBoxes[0][0][0], false },   // SBendLeft
        { &kSBendRightSprites[0][0][0], nullptr, &kSBendRightBoundBoxes[0][0][0], false }, // SBendRight
        { &kLeftVerticalLoopSprites[0][0], &kLeftVerticalLoopOffsets[0][0], &kLeftVerticalLoopBoundBoxes[0][0],
          false }, // LeftVerticalLoop
        { &kRightVerticalLoopSprites[0][0], &kRightVerticalLoopOffsets[0][0], &kRightVerticalLoopBoundBoxes[0][0],
          false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0][0], &kLeftQuarterTurn3TilesOffsets[0][0][0],
          &kLeftQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true },                   // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0],
          false },                           // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &kLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftQuarterTurn3TilesUp25Offsets[0][0][0],
          &kLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightQuarterTurn3TilesUp25Offsets[0][0][0],
          &kRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true },                        // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true },                        // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false },                       // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true },                        // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false },                       // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false },                       // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true },                        // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true },                        // RightTwistUpToDown
        { nullptr, nullptr, nullptr, false },                       // HalfLoopUp
        { nullptr, nullptr, nullptr, true },                        // HalfLoopDown
        { nullptr, nullptr, nullptr, false },                       // LeftCorkscrewUp
        { nullptr, nullptr, nullptr, false },                       // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true },                        // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true },                        // RightCorkscrewDown
        { nullptr, nullptr, nullptr, false },                       // FlatToUp60
        { nullptr, nullptr, nullptr, false },                       // Up60ToFlat
        { nullptr, nullptr, nullptr, true },                        // FlatToDown60
        { nullptr, nullptr, nullptr, true },                        // Down60ToFlat
        { nullptr, nullptr, nullptr, false },                       // TowerBase
        { nullptr, nullptr, nullptr, false },                       // TowerSection
        { nullptr, nullptr, nullptr, false },                       // FlatCovered
        { nullptr, nullptr, nullptr, false },                       // Up25Covered
        { nullptr, nullptr, nullptr, false },                       // Up60Covered
        { nullptr, nullptr, nullptr, false },                       // FlatToUp25Covered
        { nullptr, nullptr, nullptr, false },                       // Up25ToUp60Covered
        { nullptr, nullptr, nullptr, false },                       // Up60ToUp25Covered
        { nullptr, nullptr, nullptr, false },                       // Up25ToFlatCovered
        { nullptr, nullptr, nullptr, false },                       // Down25Covered
        { nullptr, nullptr, nullptr, false },                       // Down60Covered
        { nullptr, nullptr, nullptr, false },                       // FlatToDown25Covered
        { nullptr, nullptr, nullptr, false },                       // Down25ToDown60Covered
        { nullptr, nullptr, nullptr, false },                       // Down60ToDown25Covered
        { nullptr, nullptr, nullptr, false },                       // Down25ToFlatCovered
        { nullptr, nullptr, nullptr, false },                       // LeftQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr, false },                       // RightQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr, false },                       // SBendLeftCovered
        { nullptr, nullptr, nullptr, false },                       // SBendRightCovered
        { nullptr, nullptr, nullptr, false },                       // LeftQuarterTurn3TilesCovered
        { nullptr, nullptr, nullptr, false },                       // RightQuarterTurn3TilesCovered
        { &kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0],
          false }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0],
          false },                           // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0],
          false }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0],
          false },                           // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownLarge
        { &kLeftQuarterTurn1TileUp60Sprites[0][0], nullptr, &kLeftQuarterTurn1TileUp60BoundBoxes[0][0],
          false }, // LeftQuarterTurn1TileUp60
        { &kRightQuarterTurn1TileUp60Sprites[0][0], nullptr, &kRightQuarterTurn1TileUp60BoundBoxes[0][0],
          false },                                                                            // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true },                                                  // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true },                                                  // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0][0], &kBrakesOffsets[0][0], &kBrakesBoundBoxes[0][0], false },    // Brakes
        { &kBoosterSprites[0][0], &kBoosterOffsets[0][0], &kBoosterBoundBoxes[0][0], false }, // Booster
        { nullptr, nullptr, nullptr, false },                                                 // Maze
        { nullptr, nullptr, nullptr, false },                                                 // LeftQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, false },                                                 // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, true },                                                  // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, true },                                                 // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, false },                                                // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, false },                                                // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, true },                                                 // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, true },                                                 // RightQuarterHelixLargeDown
        { &kUp25LeftBankedSprites[0][0], nullptr, &kUp25LeftBankedBoundBoxes[0][0], false }, // Up25LeftBanked
        { &kUp25RightBankedSprites[0][0], nullptr, &kUp25RightBankedBoundBoxes[0][0], false },             // Up25RightBanked
        { nullptr, nullptr, nullptr, false },                                                              // Waterfall
        { nullptr, nullptr, nullptr, false },                                                              // Rapids
        { &kFlatSprites[0][0][0], &kFlatOffsets[0][0], &kFlatBoundBoxes[0][0], false },                    // OnRidePhoto
        { nullptr, nullptr, nullptr, true },                                                               // Down25LeftBanked
        { nullptr, nullptr, nullptr, true },                                                               // Down25RightBanked
        { &kWatersplashSprites[0][0][0], nullptr, &kWatersplashBoundBoxes[0][0][0], false },               // Watersplash
        { &kFlatToUp60LongBaseSprites[0][0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false },                                                              // Whirlpool
        { nullptr, nullptr, nullptr, true },  // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true },  // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false }, // CableLiftHill
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallVertical
        { nullptr, nullptr, nullptr, false }, // Up90
        { nullptr, nullptr, nullptr, true },  // Down90
        { nullptr, nullptr, nullptr, false }, // Up60ToUp90
        { nullptr, nullptr, nullptr, true },  // Down90ToDown60
        { nullptr, nullptr, nullptr, false }, // Up90ToUp60
        { nullptr, nullptr, nullptr, true },  // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &kLeftEighthToDiagSprites[0][0][0], nullptr, &kLeftEighthToDiagBoundBoxes[0][0][0], false },   // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0][0], nullptr, &kRightEighthToDiagBoundBoxes[0][0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { &kLeftEighthBankToDiagSprites[0][0][0], nullptr, &kLeftEighthBankToDiagBoundBoxes[0][0][0],
          false }, // LeftEighthBankToDiag
        { &kRightEighthBankToDiagSprites[0][0][0], nullptr, &kRightEighthBankToDiagBoundBoxes[0][0][0],
          false },                           // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0][0], &kDiagFlatOffsets[0][0][0], &kDiagFlatBoundBoxes[0][0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0][0], &kDiagUp25Offsets[0][0][0], &kDiagUp25BoundBoxes[0][0][0], false }, // DiagUp25
        { &kDiagUp60Sprites[0][0][0], &kDiagUp60Offsets[0][0][0], &kDiagUp60BoundBoxes[0][0][0], false },    // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0][0], &kDiagFlatToUp25Offsets[0][0][0], &kDiagFlatToUp25BoundBoxes[0][0][0],
          false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0][0], &kDiagUp25ToUp60Offsets[0][0][0], &kDiagUp25ToUp60BoundBoxes[0][0][0],
          false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0][0], &kDiagUp60ToUp25Offsets[0][0][0], &kDiagUp60ToUp25BoundBoxes[0][0][0],
          false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0][0], &kDiagUp25ToFlatOffsets[0][0][0], &kDiagUp25ToFlatBoundBoxes[0][0][0],
          false },                            // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true },  // DiagDown25
        { nullptr, nullptr, nullptr, true },  // DiagDown60
        { nullptr, nullptr, nullptr, true },  // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true },  // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true },  // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true },  // DiagDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagFlatToUp60
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true },  // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true },  // DiagDown60ToFlat
        { &kDiagFlatToLeftBankSprites[0][0][0], &kDiagFlatToLeftBankOffsets[0][0][0], &kDiagFlatToLeftBankBoundBoxes[0][0][0],
          false }, // DiagFlatToLeftBank
        { &kDiagFlatToRightBankSprites[0][0][0], &kDiagFlatToRightBankOffsets[0][0][0],
          &kDiagFlatToRightBankBoundBoxes[0][0][0], false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true },                 // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true },                 // DiagRightBankToFlat
        { &kDiagLeftBankToUp25Sprites[0][0][0], &kDiagLeftBankToUp25Offsets[0][0][0], &kDiagLeftBankToUp25BoundBoxes[0][0][0],
          false }, // DiagLeftBankToUp25
        { &kDiagRightBankToUp25Sprites[0][0][0], &kDiagRightBankToUp25Offsets[0][0][0],
          &kDiagRightBankToUp25BoundBoxes[0][0][0], false }, // DiagRightBankToUp25
        { &kDiagUp25ToLeftBankSprites[0][0][0], &kDiagUp25ToLeftBankOffsets[0][0][0], &kDiagUp25ToLeftBankBoundBoxes[0][0][0],
          false }, // DiagUp25ToLeftBank
        { &kDiagUp25ToRightBankSprites[0][0][0], &kDiagUp25ToRightBankOffsets[0][0][0],
          &kDiagUp25ToRightBankBoundBoxes[0][0][0], false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true },                 // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true },                 // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true },                 // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true },                 // DiagDown25ToRightBank
        { &kDiagLeftBankSprites[0][0][0], &kDiagLeftBankOffsets[0][0][0], &kDiagLeftBankBoundBoxes[0][0][0],
          false },                            // DiagLeftBank
        { nullptr, nullptr, nullptr, true },  // DiagRightBank
        { nullptr, nullptr, nullptr, false }, // LogFlumeReverser
        { nullptr, nullptr, nullptr, false }, // SpinningTunnel
        { nullptr, nullptr, nullptr, false }, // LeftBarrelRollUpToDown
        { nullptr, nullptr, nullptr, false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true },  // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true },  // RightBarrelRollDownToUp
        { &kLeftBankToLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25Offsets[0][0][0],
          &kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { &kRightBankToRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25Offsets[0][0][0],
          &kRightBankToRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true },                                   // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true },                                   // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false },                                  // PoweredLift
        { &kLeftLargeHalfLoopUpSprites[0][0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0][0],
          false }, // LeftLargeHalfLoopUp
        { &kRightLargeHalfLoopUpSprites[0][0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0][0],
          false },                            // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr, true },  // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr, true },  // RightLargeHalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftFlyerTwistUp
        { nullptr, nullptr, nullptr, false }, // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, true },  // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, true },  // RightFlyerTwistDown
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false }, // FlyerHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, true },  // LeftFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, true },  // RightFlyerCorkscrewUp
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
        { nullptr, nullptr, nullptr, true },  // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true },  // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true },  // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0][0], &kBlockBrakesOffsets[0][0], &kBlockBrakesBoundBoxes[0][0], false }, // BlockBrakes
        { &kLeftBankedQuarterTurn3TileUp25Sprites[0][0][0], &kLeftBankedQuarterTurn3TileUp25Offsets[0][0][0],
          &kLeftBankedQuarterTurn3TileUp25BoundBoxes[0][0][0], false }, // LeftBankedQuarterTurn3TileUp25
        { &kRightBankedQuarterTurn3TileUp25Sprites[0][0][0], &kRightBankedQuarterTurn3TileUp25Offsets[0][0][0],
          &kRightBankedQuarterTurn3TileUp25BoundBoxes[0][0][0], false }, // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true },                             // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true },                             // RightBankedQuarterTurn3TileDown25
        { &kLeftBankedQuarterTurn5TileUp25Sprites[0][0][0], nullptr, &kLeftBankedQuarterTurn5TileUp25BoundBoxes[0][0][0],
          false }, // LeftBankedQuarterTurn5TileUp25
        { &kRightBankedQuarterTurn5TileUp25Sprites[0][0][0], nullptr, &kRightBankedQuarterTurn5TileUp25BoundBoxes[0][0][0],
          false },                           // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn5TileDown25
        { &kUp25ToLeftBankedUp25Sprites[0][0], nullptr, &kUp25ToLeftBankedUp25BoundBoxes[0][0], false }, // Up25ToLeftBankedUp25
        { &kUp25ToRightBankedUp25Sprites[0][0], nullptr, &kUp25ToRightBankedUp25BoundBoxes[0][0],
          false }, // Up25ToRightBankedUp25
        { &kLeftBankedUp25ToUp25Sprites[0][0], nullptr, &kLeftBankedUp25ToUp25BoundBoxes[0][0], false }, // LeftBankedUp25ToUp25
        { &kRightBankedUp25ToUp25Sprites[0][0], nullptr, &kRightBankedUp25ToUp25BoundBoxes[0][0],
          false },                           // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToDown25
        { &kLeftBankedFlatToLeftBankedUp25Sprites[0][0], nullptr, &kLeftBankedFlatToLeftBankedUp25BoundBoxes[0][0],
          false }, // LeftBankedFlatToLeftBankedUp25
        { &kRightBankedFlatToRightBankedUp25Sprites[0][0], nullptr, &kRightBankedFlatToRightBankedUp25BoundBoxes[0][0],
          false }, // RightBankedFlatToRightBankedUp25
        { &kLeftBankedUp25ToLeftBankedFlatSprites[0][0], nullptr, &kLeftBankedUp25ToLeftBankedFlatBoundBoxes[0][0],
          false }, // LeftBankedUp25ToLeftBankedFlat
        { &kRightBankedUp25ToRightBankedFlatSprites[0][0], nullptr, &kRightBankedUp25ToRightBankedFlatBoundBoxes[0][0],
          false },                           // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToRightBankedFlat
        { &kFlatToLeftBankedUp25Sprites[0][0], nullptr, &kFlatToLeftBankedUp25BoundBoxes[0][0], false }, // FlatToLeftBankedUp25
        { &kFlatToRightBankedUp25Sprites[0][0], nullptr, &kFlatToRightBankedUp25BoundBoxes[0][0],
          false }, // FlatToRightBankedUp25
        { &kLeftBankedUp25ToFlatSprites[0][0], nullptr, &kLeftBankedUp25ToFlatBoundBoxes[0][0], false }, // LeftBankedUp25ToFlat
        { &kRightBankedUp25ToFlatSprites[0][0], nullptr, &kRightBankedUp25ToFlatBoundBoxes[0][0],
          false },                            // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true },  // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true },  // RightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, true },  // LeftQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr, true },  // RightQuarterTurn1TileDown90
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
        { nullptr, nullptr, nullptr, true },  // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true },  // RightLargeCorkscrewDown
        { &kLeftMediumHalfLoopUpSprites[0][0][0], nullptr, &kLeftMediumHalfLoopUpBoundBoxes[0][0][0],
          false }, // LeftMediumHalfLoopUp
        { &kRightMediumHalfLoopUpSprites[0][0][0], nullptr, &kRightMediumHalfLoopUpBoundBoxes[0][0][0],
          false },                            // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true },  // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true },  // RightMediumHalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftZeroGRollUp
        { nullptr, nullptr, nullptr, false }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true },  // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true },  // RightZeroGRollDown
        { nullptr, nullptr, nullptr, false }, // LeftLargeZeroGRollUp
        { nullptr, nullptr, nullptr, false }, // RightLargeZeroGRollUp
        { nullptr, nullptr, nullptr, true },  // LeftLargeZeroGRollDown
        { nullptr, nullptr, nullptr, true },  // RightLargeZeroGRollDown
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
        { nullptr, nullptr, nullptr, true },  // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthToDiagDown25
        { nullptr, nullptr, nullptr, false }, // LeftEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, true },  // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true },  // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedDown25ToDown25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr, false }, // DiagFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true },  // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagUp25LeftBanked
        { nullptr, nullptr, nullptr, false }, // DiagUp25RightBanked
        { nullptr, nullptr, nullptr, true },  // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr, true },  // DiagDown25RightBanked
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, true },  // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, true },  // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthBankToOrthogonalDown25
        { &kDiagBrakesSprites[0][0][0], &kDiagBrakesOffsets[0][0][0], &kDiagBrakesBoundBoxes[0][0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0][0], &kDiagBlockBrakesOffsets[0][0][0], &kDiagBlockBrakesBoundBoxes[0][0][0],
          false },                            // DiagBlockBrakes
        { nullptr, nullptr, nullptr, false }, // Down25Brakes
        { nullptr, nullptr, nullptr, false }, // DiagBooster
        { nullptr, nullptr, nullptr, false }, // DiagFlatToUp60LongBase
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr, true },  // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr, true },  // DiagDown60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // LeftEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, false }, // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, true },  // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, true },  // RightEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, false }, // DiagDown25Brakes
    } };
} // namespace OpenRCT2::WoodenRollerCoaster
