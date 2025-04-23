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

namespace OpenRCT2::LatticeTriangle
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 3>
    kFlatSprites = {{
        {{
            18074,
            18075,
            18074,
            18075,
        }},
        {{
            18382,
            18383,
            18384,
            18385,
        }},
        {{
            18692,
            18693,
            18692,
            18693,
        }},
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kEndStationSprites = {{
        {{
            18076,
            18077,
            18076,
            18077,
        }},
        {{
            18078,
            18079,
            18078,
            18079,
        }},
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kStationSprites = {{
        18084,
        18085,
        18084,
        18085,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 3>
    kUp25Sprites = {{
        {{
            18134,
            18135,
            18136,
            18137,
        }},
        {{
            18394,
            18395,
            18396,
            18397,
        }},
        {{
            18702,
            18703,
            18704,
            18705,
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
            18150,
            18151,
            18152,
            18153,
        }},
        {{
            18718,
            18719,
            18720,
            18721,
        }},
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 27, 0, 0 }, { 1, 32, 98 } },
        { { 0, 27, 0 }, { 32, 1, 98 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 3>
    kFlatToUp25Sprites = {{
        {{
            18126,
            18127,
            18128,
            18129,
        }},
        {{
            18386,
            18387,
            18388,
            18389,
        }},
        {{
            18694,
            18695,
            18696,
            18697,
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
            {{ 18138, kImageIndexUndefined, }},
            {{ 18139, 18142, }},
            {{ 18140, 18143, }},
            {{ 18141, kImageIndexUndefined, }},
        }},
        {{
            {{ 18706, kImageIndexUndefined, }},
            {{ 18707, 18710, }},
            {{ 18708, 18711, }},
            {{ 18709, kImageIndexUndefined, }},
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
            {{ 18144, kImageIndexUndefined, }},
            {{ 18145, 18148, }},
            {{ 18146, 18149, }},
            {{ 18147, kImageIndexUndefined, }},
        }},
        {{
            {{ 18712, kImageIndexUndefined, }},
            {{ 18713, 18716, }},
            {{ 18714, 18717, }},
            {{ 18715, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};

    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 3>
    kUp25ToFlatSprites = {{
        {{
            18130,
            18131,
            18132,
            18133,
        }},
        {{
            18390,
            18391,
            18392,
            18393,
        }},
        {{
            18698,
            18699,
            18700,
            18701,
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
            18189,
            kImageIndexUndefined,
            18188,
            18187,
            kImageIndexUndefined,
            18186,
            18185,
        }},
        {{
            18194,
            kImageIndexUndefined,
            18193,
            18192,
            kImageIndexUndefined,
            18191,
            18190,
        }},
        {{
            18199,
            kImageIndexUndefined,
            18198,
            18197,
            kImageIndexUndefined,
            18196,
            18195,
        }},
        {{
            18184,
            kImageIndexUndefined,
            18183,
            18182,
            kImageIndexUndefined,
            18181,
            18180,
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
        {{ 18086, 18094, }},
        {{ 18087, 18095, }},
        {{ 18088, kImageIndexUndefined, }},
        {{ 18089, kImageIndexUndefined, }},
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
        {{ 18090, kImageIndexUndefined, }},
        {{ 18091, kImageIndexUndefined, }},
        {{ 18092, 18096, }},
        {{ 18093, 18097, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 18209, 18220, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18208, kImageIndexUndefined, }},
            {{ 18207, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18206, kImageIndexUndefined, }},
            {{ 18205, kImageIndexUndefined, }},
        }},
        {{
            {{ 18214, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18213, kImageIndexUndefined, }},
            {{ 18212, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18211, kImageIndexUndefined, }},
            {{ 18210, kImageIndexUndefined, }},
        }},
        {{
            {{ 18219, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18218, kImageIndexUndefined, }},
            {{ 18217, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18216, kImageIndexUndefined, }},
            {{ 18215, 18221, }},
        }},
        {{
            {{ 18204, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18203, kImageIndexUndefined, }},
            {{ 18202, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18201, kImageIndexUndefined, }},
            {{ 18200, kImageIndexUndefined, }},
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
        {{ 18098, 18102, }},
        {{ 18099, 18103, }},
        {{ 18100, kImageIndexUndefined, }},
        {{ 18101, kImageIndexUndefined, }},
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
        {{ 18104, kImageIndexUndefined, }},
        {{ 18105, kImageIndexUndefined, }},
        {{ 18106, 18108, }},
        {{ 18107, 18109, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 18110, 18114, }},
        {{ 18111, 18115, }},
        {{ 18112, kImageIndexUndefined, }},
        {{ 18113, kImageIndexUndefined, }},
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
        {{ 18116, kImageIndexUndefined, }},
        {{ 18117, kImageIndexUndefined, }},
        {{ 18118, 18120, }},
        {{ 18119, 18121, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        18122,
        18123,
        18124,
        18125,
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
            18274,
            kImageIndexUndefined,
            18275,
            18276,
            kImageIndexUndefined,
            18277,
            18278,
        }},
        {{
            18279,
            kImageIndexUndefined,
            18280,
            18281,
            kImageIndexUndefined,
            18282,
            18283,
        }},
        {{
            18284,
            kImageIndexUndefined,
            18285,
            18286,
            kImageIndexUndefined,
            18287,
            18288,
        }},
        {{
            18289,
            kImageIndexUndefined,
            18290,
            18291,
            kImageIndexUndefined,
            18292,
            18293,
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
            18254,
            kImageIndexUndefined,
            18255,
            18256,
            kImageIndexUndefined,
            18257,
            18258,
        }},
        {{
            18259,
            kImageIndexUndefined,
            18260,
            18261,
            kImageIndexUndefined,
            18262,
            18263,
        }},
        {{
            18264,
            kImageIndexUndefined,
            18265,
            18266,
            kImageIndexUndefined,
            18267,
            18268,
        }},
        {{
            18269,
            kImageIndexUndefined,
            18270,
            18271,
            kImageIndexUndefined,
            18272,
            18273,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            18238,
            18239,
            18240,
            18241,
        }},
        {{
            18242,
            18243,
            18244,
            18245,
        }},
        {{
            18241,
            18240,
            18239,
            18238,
        }},
        {{
            18245,
            18244,
            18243,
            18242,
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
            18246,
            18247,
            18248,
            18249,
        }},
        {{
            18250,
            18251,
            18252,
            18253,
        }},
        {{
            18249,
            18248,
            18247,
            18246,
        }},
        {{
            18253,
            18252,
            18251,
            18250,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 3,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 11,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 8,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 7,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 15,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 14,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 12,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 11,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 0,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 12,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 14,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 15,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 7,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 4,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 17,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 19,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 27,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 26,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 24,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 20,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 22,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 23,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 31,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 30,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 29,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 28,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 24,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 26,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 27,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 19,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 17,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 16,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 28,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 29,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 30,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 31,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 23,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 22,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_LOOP + 20,
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
            18159,
            kImageIndexUndefined,
            18158,
            18157,
        }},
        {{
            18162,
            kImageIndexUndefined,
            18161,
            18160,
        }},
        {{
            18165,
            kImageIndexUndefined,
            18164,
            18163,
        }},
        {{
            18156,
            kImageIndexUndefined,
            18155,
            18154,
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
            {{ 18171, 18178, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18170, kImageIndexUndefined, }},
            {{ 18169, kImageIndexUndefined, }},
        }},
        {{
            {{ 18174, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18173, kImageIndexUndefined, }},
            {{ 18172, kImageIndexUndefined, }},
        }},
        {{
            {{ 18177, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18176, kImageIndexUndefined, }},
            {{ 18175, 18179, }},
        }},
        {{
            {{ 18168, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18167, kImageIndexUndefined, }},
            {{ 18166, kImageIndexUndefined, }},
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
            18305,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18304,
        }},
        {{
            18307,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18306,
        }},
        {{
            18309,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18308,
        }},
        {{
            18303,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18302,
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
            18294,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18295,
        }},
        {{
            18296,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18297,
        }},
        {{
            18298,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18299,
        }},
        {{
            18300,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18301,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 3,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 7,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 11,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 12,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 14,
            SPR_G2_LATTICE_TRIANGLE_TRACK_HALF_LOOP + 15,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 2,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 5,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 7,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 8,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 11,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 12,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 14,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 15,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 17,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 19,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 20,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 22,
            SPR_G2_LATTICE_TRIANGLE_TRACK_CORKSCREW + 23,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp60Sprites = {{
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 2, SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 1, }},
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 4, SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 3, }},
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 4 } }, {} }},
        {{ { { 4, 29, 2 }, { 24, 1, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 29, 4, 2 }, { 1, 24, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 4 } }, {} }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToFlatSprites = {{
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 6, kImageIndexUndefined, }},
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 7, SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 8, }},
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 10, SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 9, }},
        {{ SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 11, kImageIndexUndefined, }},
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
            {{ 18329, 18336, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18328, kImageIndexUndefined, }},
            {{ 18327, kImageIndexUndefined, }},
            {{ 18326, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18325, kImageIndexUndefined, }},
            {{ 18324, kImageIndexUndefined, }},
        }},
        {{
            {{ 18332, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18331, kImageIndexUndefined, }},
            {{ 18330, kImageIndexUndefined, }},
            {{ 18329, 18336, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18328, kImageIndexUndefined, }},
            {{ 18327, kImageIndexUndefined, }},
        }},
        {{
            {{ 18335, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18334, kImageIndexUndefined, }},
            {{ 18333, 18337, }},
            {{ 18332, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18331, kImageIndexUndefined, }},
            {{ 18330, kImageIndexUndefined, }},
        }},
        {{
            {{ 18326, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18325, kImageIndexUndefined, }},
            {{ 18324, kImageIndexUndefined, }},
            {{ 18335, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18334, kImageIndexUndefined, }},
            {{ 18333, 18337, }},
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
            {{ 18310, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18311, kImageIndexUndefined, }},
            {{ 18312, kImageIndexUndefined, }},
            {{ 18313, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18314, kImageIndexUndefined, }},
            {{ 18315, 18322, }},
        }},
        {{
            {{ 18313, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18314, kImageIndexUndefined, }},
            {{ 18315, 18322, }},
            {{ 18316, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18317, kImageIndexUndefined, }},
            {{ 18318, kImageIndexUndefined, }},
        }},
        {{
            {{ 18316, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18317, kImageIndexUndefined, }},
            {{ 18318, kImageIndexUndefined, }},
            {{ 18319, 18323, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18320, kImageIndexUndefined, }},
            {{ 18321, kImageIndexUndefined, }},
        }},
        {{
            {{ 18319, 18323, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18320, kImageIndexUndefined, }},
            {{ 18321, kImageIndexUndefined, }},
            {{ 18310, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18311, kImageIndexUndefined, }},
            {{ 18312, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 18369, 18380, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18368, kImageIndexUndefined, }},
            {{ 18367, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18366, kImageIndexUndefined, }},
            {{ 18365, kImageIndexUndefined, }},
            {{ 18364, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18363, kImageIndexUndefined, }},
            {{ 18362, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18361, kImageIndexUndefined, }},
            {{ 18360, kImageIndexUndefined, }},
        }},
        {{
            {{ 18374, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18373, kImageIndexUndefined, }},
            {{ 18372, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18371, kImageIndexUndefined, }},
            {{ 18370, kImageIndexUndefined, }},
            {{ 18369, 18380, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18368, kImageIndexUndefined, }},
            {{ 18367, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18366, kImageIndexUndefined, }},
            {{ 18365, kImageIndexUndefined, }},
        }},
        {{
            {{ 18379, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18378, kImageIndexUndefined, }},
            {{ 18377, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18376, kImageIndexUndefined, }},
            {{ 18375, 18381, }},
            {{ 18374, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18373, kImageIndexUndefined, }},
            {{ 18372, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18371, kImageIndexUndefined, }},
            {{ 18370, kImageIndexUndefined, }},
        }},
        {{
            {{ 18364, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18363, kImageIndexUndefined, }},
            {{ 18362, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18361, kImageIndexUndefined, }},
            {{ 18360, kImageIndexUndefined, }},
            {{ 18379, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18378, kImageIndexUndefined, }},
            {{ 18377, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18376, kImageIndexUndefined, }},
            {{ 18375, 18381, }},
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
            {{ 18338, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18339, kImageIndexUndefined, }},
            {{ 18340, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18341, kImageIndexUndefined, }},
            {{ 18342, kImageIndexUndefined, }},
            {{ 18343, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18344, kImageIndexUndefined, }},
            {{ 18345, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18346, kImageIndexUndefined, }},
            {{ 18347, 18358, }},
        }},
        {{
            {{ 18343, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18344, kImageIndexUndefined, }},
            {{ 18345, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18346, kImageIndexUndefined, }},
            {{ 18347, 18358, }},
            {{ 18348, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18349, kImageIndexUndefined, }},
            {{ 18350, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18351, kImageIndexUndefined, }},
            {{ 18352, kImageIndexUndefined, }},
        }},
        {{
            {{ 18348, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18349, kImageIndexUndefined, }},
            {{ 18350, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18351, kImageIndexUndefined, }},
            {{ 18352, kImageIndexUndefined, }},
            {{ 18353, 18359, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18354, kImageIndexUndefined, }},
            {{ 18355, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18356, kImageIndexUndefined, }},
            {{ 18357, kImageIndexUndefined, }},
        }},
        {{
            {{ 18353, 18359, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18354, kImageIndexUndefined, }},
            {{ 18355, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18356, kImageIndexUndefined, }},
            {{ 18357, kImageIndexUndefined, }},
            {{ 18338, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18339, kImageIndexUndefined, }},
            {{ 18340, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18341, kImageIndexUndefined, }},
            {{ 18342, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ 18231, 18235, }},
        {{ 18232, 18236, }},
        {{ 18233, 18237, }},
        {{ 18230, 18234, }},
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
        {{ 18222, 18226, }},
        {{ 18223, 18227, }},
        {{ 18224, 18228, }},
        {{ 18225, 18229, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBrakesSprites = {{
        {{ 18078, 18082, }},
        {{ 18079, 18083, }},
        {{ 18078, 18082, }},
        {{ 18079, 18083, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 5 }, { 32, 1, 11 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 5 }, { 1, 32, 11 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 5 }, { 32, 1, 11 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 5 }, { 1, 32, 11 } } }},
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBoosterSprites = {{
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_NE_SW + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_NW_SE + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_NE_SW + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_BOOSTER_NW_SE + 0,
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
        18560,
        18561,
        18562,
        18563,
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
        18564,
        18565,
        18566,
        18567,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kFlatToUp60LongBaseSprites = {{
        {{
            {{
                18660,
                18661,
                18662,
                18663,
            }},
            {{
                18664,
                18665,
                18666,
                18667,
            }},
            {{
                18668,
                18669,
                18670,
                18671,
            }},
            {{
                18672,
                18673,
                18674,
                18675,
            }},
        }},
        {{
            {{
                18722,
                18723,
                18724,
                18725,
            }},
            {{
                18726,
                18727,
                18728,
                18729,
            }},
            {{
                18730,
                18731,
                18732,
                18733,
            }},
            {{
                18734,
                18735,
                18736,
                18737,
            }},
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
            18676,
            18677,
            18678,
            18679,
        }},
        {{
            18680,
            18681,
            18682,
            18683,
        }},
        {{
            18684,
            18685,
            18686,
            18687,
        }},
        {{
            18688,
            18689,
            18690,
            18691,
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

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kCableLiftHillSprites = {{
        {{
            {{ 18698, kImageIndexUndefined, }},
            {{ 18700, kImageIndexUndefined, }},
            {{ 18714, 18717, }},
            {{ 18720, kImageIndexUndefined, }},
        }},
        {{
            {{ 18699, kImageIndexUndefined, }},
            {{ 18701, kImageIndexUndefined, }},
            {{ 18715, kImageIndexUndefined, }},
            {{ 18721, kImageIndexUndefined, }},
        }},
        {{
            {{ 18700, kImageIndexUndefined, }},
            {{ 18698, kImageIndexUndefined, }},
            {{ 18712, kImageIndexUndefined, }},
            {{ 18718, kImageIndexUndefined, }},
        }},
        {{
            {{ 18701, kImageIndexUndefined, }},
            {{ 18699, kImageIndexUndefined, }},
            {{ 18713, 18716, }},
            {{ 18719, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kCableLiftHillBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
            {{ { { 0, 27, 0 }, { 32, 1, 98 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
            {{ { { 27, 0, 0 }, { 1, 32, 98 } }, {} }},
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp90Sprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 9,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 10,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 11,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 1,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 2,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 3,
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
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 2,
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 3,
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 1,
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
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 4,
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 5,
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 6,
        SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 7,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 6,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 7,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 4,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL + 5,
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

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            18414,
            18415,
            18416,
            kImageIndexUndefined,
            18417,
        }},
        {{
            18418,
            18419,
            18420,
            kImageIndexUndefined,
            18421,
        }},
        {{
            18422,
            18423,
            18424,
            kImageIndexUndefined,
            18425,
        }},
        {{
            18426,
            18427,
            18428,
            kImageIndexUndefined,
            18429,
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
            18398,
            18399,
            18400,
            kImageIndexUndefined,
            18401,
        }},
        {{
            18402,
            18403,
            18404,
            kImageIndexUndefined,
            18405,
        }},
        {{
            18406,
            18407,
            18408,
            kImageIndexUndefined,
            18409,
        }},
        {{
            18410,
            18411,
            18412,
            kImageIndexUndefined,
            18413,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            18446,
            18447,
            18448,
            kImageIndexUndefined,
            18449,
        }},
        {{
            18450,
            18451,
            18452,
            kImageIndexUndefined,
            18453,
        }},
        {{
            18454,
            18455,
            18456,
            kImageIndexUndefined,
            18457,
        }},
        {{
            18458,
            18459,
            18460,
            kImageIndexUndefined,
            18461,
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
            18430,
            18431,
            18432,
            kImageIndexUndefined,
            18433,
        }},
        {{
            18434,
            18435,
            18436,
            kImageIndexUndefined,
            18437,
        }},
        {{
            18438,
            18439,
            18440,
            kImageIndexUndefined,
            18441,
        }},
        {{
            18442,
            18443,
            18444,
            kImageIndexUndefined,
            18445,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                18462,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18463,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18464,
                kImageIndexUndefined,
            }},
            {{
                18465,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                18524,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18525,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18526,
                kImageIndexUndefined,
            }},
            {{
                18527,
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
                18474,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18475,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18476,
                kImageIndexUndefined,
            }},
            {{
                18477,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                18536,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18537,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18538,
                kImageIndexUndefined,
            }},
            {{
                18539,
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

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            kImageIndexUndefined,
            18486,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18487,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            18488,
            kImageIndexUndefined,
        }},
        {{
            18489,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
                18466,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18467,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18468,
                kImageIndexUndefined,
            }},
            {{
                18469,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                18528,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18529,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18530,
                kImageIndexUndefined,
            }},
            {{
                18531,
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

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            kImageIndexUndefined,
            18478,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18479,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            18480,
            kImageIndexUndefined,
        }},
        {{
            18481,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            18482,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18483,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            18484,
            kImageIndexUndefined,
        }},
        {{
            18485,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
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
                18470,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18471,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18472,
                kImageIndexUndefined,
            }},
            {{
                18473,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                18532,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                18533,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                18534,
                kImageIndexUndefined,
            }},
            {{
                18535,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 13,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 14,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 15,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 18,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_SMALL_FLAT_TO_STEEP + 19,
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
            {{ 18494, 18498, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18495, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18496, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18497, kImageIndexUndefined, }},
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
            {{ 18499, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18500, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18501, 18503, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18502, kImageIndexUndefined, }},
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
            {{ 18514, 18518, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18515, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18516, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18517, kImageIndexUndefined, }},
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
            {{ 18519, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18520, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18521, 18523, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18522, kImageIndexUndefined, }},
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
            {{ 18504, 18508, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18505, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18506, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18507, kImageIndexUndefined, }},
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
            {{ 18509, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18510, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 18511, 18513, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 18512, kImageIndexUndefined, }},
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
            18490,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18491,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            18492,
            kImageIndexUndefined,
        }},
        {{
            18493,
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 1, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 2, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 3, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 4, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 5, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 6, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 7, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 8, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 9, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 10, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 12, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 13, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 14, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 15, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 16, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 17, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 18, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 19, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 20, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 21, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 22, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 23, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 24, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 25, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 26, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 27, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 28, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 29, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 30, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 31, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 32, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 33, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 34, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 35, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 36, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 37, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 38, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 39, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 40, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 41, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 42, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 43, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 44, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 45, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 46, SPR_G2_LATTICE_TRIANGLE_TRACK_BARREL_ROLL + 47, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBarrelRollUpToDownBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 1, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 2, SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 8, kImageIndexUndefined, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 13, SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 14, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 16, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_TURN_BANK_TRANSITION + 17, kImageIndexUndefined, }},
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
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_1 + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_2 + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_3 + 0,
        SPR_G2_LATTICE_TRIANGLE_TRACK_POWERED_LIFT_4 + 0,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 6,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 7,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 11,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 12,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 13,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 14,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 15,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 17,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 19,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 20,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 22,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 23,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 24,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 26,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 27,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 28,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 29,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 30,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 31,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 32,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 33,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 34,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 35,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 36,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 37,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 38,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 39,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 40,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 41,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 42,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 43,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 44,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 45,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 46,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 47,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 48,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 49,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 50,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 51,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 52,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 53,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 54,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_HALF_LOOP + 55,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 2,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 5,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 7,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 8,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 11,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 3,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 7,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 6,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 11,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_QUARTER_LOOP + 9,
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

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            {{ 18076, 18080, }},
            {{ 18077, 18081, }},
            {{ 18076, 18080, }},
            {{ 18077, 18081, }},
        }},
        {{
            {{ 18078, 18082, }},
            {{ 18079, 18083, }},
            {{ 18078, 18082, }},
            {{ 18079, 18083, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 5 }, { 32, 1, 11 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 5 }, { 1, 32, 11 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 5 }, { 32, 1, 11 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 5 }, { 1, 32, 11 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Sprites = {{
        {{
            18655,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18654,
        }},
        {{
            18657,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18656,
        }},
        {{
            18659,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18658,
        }},
        {{
            18653,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18652,
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
            18644,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18645,
        }},
        {{
            18646,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18647,
        }},
        {{
            18648,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18649,
        }},
        {{
            18650,
            kImageIndexUndefined,
            kImageIndexUndefined,
            18651,
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
            18624,
            kImageIndexUndefined,
            18625,
            18626,
            kImageIndexUndefined,
            18627,
            18628,
        }},
        {{
            18629,
            kImageIndexUndefined,
            18630,
            18631,
            kImageIndexUndefined,
            18632,
            18633,
        }},
        {{
            18634,
            kImageIndexUndefined,
            18635,
            18636,
            kImageIndexUndefined,
            18637,
            18638,
        }},
        {{
            18639,
            kImageIndexUndefined,
            18640,
            18641,
            kImageIndexUndefined,
            18642,
            18643,
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
            18604,
            kImageIndexUndefined,
            18605,
            18606,
            kImageIndexUndefined,
            18607,
            18608,
        }},
        {{
            18609,
            kImageIndexUndefined,
            18610,
            18611,
            kImageIndexUndefined,
            18612,
            18613,
        }},
        {{
            18614,
            kImageIndexUndefined,
            18615,
            18616,
            kImageIndexUndefined,
            18617,
            18618,
        }},
        {{
            18619,
            kImageIndexUndefined,
            18620,
            18621,
            kImageIndexUndefined,
            18622,
            18623,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ 18568, kImageIndexUndefined, }},
        {{ 18569, 18576, }},
        {{ 18570, kImageIndexUndefined, }},
        {{ 18571, kImageIndexUndefined, }},
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
        {{ 18572, kImageIndexUndefined, }},
        {{ 18573, kImageIndexUndefined, }},
        {{ 18574, 18577, }},
        {{ 18575, kImageIndexUndefined, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ 18578, kImageIndexUndefined, }},
        {{ 18579, 18586, }},
        {{ 18580, kImageIndexUndefined, }},
        {{ 18581, kImageIndexUndefined, }},
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
        {{ 18582, kImageIndexUndefined, }},
        {{ 18583, kImageIndexUndefined, }},
        {{ 18584, 18587, }},
        {{ 18585, kImageIndexUndefined, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        18588,
        18589,
        18590,
        18591,
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
        18592,
        18593,
        18594,
        18595,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        18596,
        18597,
        18598,
        18599,
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
        18600,
        18601,
        18602,
        18603,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ 18540, kImageIndexUndefined, }},
        {{ 18541, 18548, }},
        {{ 18542, kImageIndexUndefined, }},
        {{ 18543, kImageIndexUndefined, }},
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
        {{ 18544, kImageIndexUndefined, }},
        {{ 18545, kImageIndexUndefined, }},
        {{ 18546, 18549, }},
        {{ 18547, kImageIndexUndefined, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ 18550, kImageIndexUndefined, }},
        {{ 18551, 18558, }},
        {{ 18552, kImageIndexUndefined, }},
        {{ 18553, kImageIndexUndefined, }},
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
        {{ 18554, kImageIndexUndefined, }},
        {{ 18555, kImageIndexUndefined, }},
        {{ 18556, 18559, }},
        {{ 18557, kImageIndexUndefined, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 1, SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 2, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 4, SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 5, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 6, SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 7, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 8, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 9, SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 10, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_VERTICAL_TWIST + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 2,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 4,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 7,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 9,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 11,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 12,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 14,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 15,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 17,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 19,
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
            { { 29, 0, 0 }, { 1, 26, 32 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
            {},
            { { 2, 2, 50 }, { 28, 28, 0 } },
            { { 2, 2, 40 }, { 28, 28, 1 } },
        }},
        {{
            { { 0, 6, 4 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 20, 20, 3 } },
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 20,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 22,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 23,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 24,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 26,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 27,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 28,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 29,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 30,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 31,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 32,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 33,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 34,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 35,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 36,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 37,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 38,
            SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_CORKSCREW + 39,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeCorkscrewUpBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 3,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 4,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 7,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 9,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 11,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 12,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 14,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 15,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 17,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 19,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 0, 2 }, { 32, 32, 0 } },
            { { 2, 0, 0 }, { 1, 32, 160 } },
            { { 0, 2, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 31, 0, 0 }, { 32, 0, 64 } },
            { { 0, 29, 0 }, { 32, 1, 96 } },
            { { 24, 30, 0 }, { 8, 0, 160 } },
            { { 2, 0, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 32, 0 }, { 40, 0, 64 } },
            { { 31, 0, 0 }, { 1, 32, 96 } },
            { { 29, 0, 0 }, { 1, 32, 160 } },
            { { 0, 0, 48 }, { 32, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 0, 0, 0 }, { 32, 32, 0 } },
            { { 0, 0, 0 }, { 32, 1, 160 } },
            { { 0, 0, 48 }, { 32, 32, 1 } },
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpSprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 20,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 22,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 23,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 24,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 26,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 27,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 28,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 29,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 30,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 31,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 32,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 33,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 34,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 35,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 36,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 37,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 38,
            SPR_G2_LATTICE_TRIANGLE_TRACK_MEDIUM_HALF_LOOP + 39,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 1, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 2, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 4, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 5, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 9, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 10, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 14, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 15, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 17, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 18, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 19, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 22, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 23, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 24, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 25, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 26, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 28, SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 29, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftZeroGRollUpBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 0, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 1, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 2, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 5, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 9, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 10, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 11, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 13, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 14, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 15, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 17, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 18, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 19, kImageIndexUndefined, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 22, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 23, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 25, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 26, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 28, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 29, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 30, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 32, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 33, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 35, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 36, SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 37, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 38, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_LARGE_ZERO_G_ROLL + 39, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeZeroGRollUpBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagUp25Sprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 0,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 2,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 3,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 4,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 6,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 7,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 10,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 11,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 12,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 14,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 15,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 17,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 18,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 19,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 20,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 22,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 23,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 24,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 26,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 27,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 28,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 29,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 30,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 31,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 32,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 33,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 34,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 35,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 36,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 37,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 38,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 39,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 40,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 41,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 42,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 43,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 44,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 45,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 46,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 47,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 48,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 49,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 50,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 51,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 52,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 53,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 54,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 55,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 56,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 57,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 58,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 59,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 60,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 61,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 62,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE + 63,
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 2, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 3, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 4, kImageIndexUndefined, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 7, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 9, kImageIndexUndefined, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 10, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 11, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 13, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 14, kImageIndexUndefined, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 15, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 16, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 17, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 18, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 19, kImageIndexUndefined, }},
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 21,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 22,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 23,
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

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagRightBankedFlatToRightBankedUp25Sprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 24,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 25,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 26,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 27,
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
            { { -16, -16, 27 }, { 32, 32, 0 } },
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 29,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 30,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 31,
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

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagRightBankedUp25ToRightBankedFlatSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 32,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 33,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 34,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 35,
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
            { { -16, -16, 27 }, { 32, 32, 0 } },
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 44, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 45, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 46, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 47, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 48, kImageIndexUndefined, }},
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
    kDiagFlatToRightBankedUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 49, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 50, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 51, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 52, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 53, kImageIndexUndefined, }},
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
    kDiagLeftBankedUp25ToFlatSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 54, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 55, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 56, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 57, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 58, kImageIndexUndefined, }},
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
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 59, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 60, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 61, SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 62, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 63, kImageIndexUndefined, }},
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 36,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 37,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 38,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 39,
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
            { { -16, -16, 48 }, { 32, 32, 0 } },
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 40,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 41,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 42,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 43,
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
            { { -16, -16, 48 }, { 32, 32, 0 } },
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 64,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 65,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 66,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 67,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 68,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 69,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 70,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 71,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 72,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 73,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 74,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 75,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 76,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 77,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 78,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 79,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 80,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 81,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 82,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 83,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 84,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 85,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 86,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 87,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 88,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 89,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 90,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 91,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 92,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 93,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 94,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 95,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagUp25BoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25Sprites = {{
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 96,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 97,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 98,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 99,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 100,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 101,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 102,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 103,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 104,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 105,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 106,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 107,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 108,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 109,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 110,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 111,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
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
            { { 0, 0, 0 }, { 32, 16, 10 } },
            { { 0, 0, 0 }, { 32, 20, 10 } },
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 112,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 113,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 114,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 115,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 116,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 117,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 118,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 119,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 120,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 121,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 122,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 123,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 124,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 125,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 126,
            SPR_G2_LATTICE_TRIANGLE_TRACK_GENTLE_LARGE_CURVE_BANKED + 127,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToOrthogonalUp25BoundBoxes = {{
        {{
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 10 } },
            { { 6, 0, 0 }, { 20, 32, 10 } },
        }},
        {{
            { { 0, 0, 32 }, { 32, 32, 1 } },
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
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};

    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>, 2>
    kDiagBrakesSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 4, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 4, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 1, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 5, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 1, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 5, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagBrakesOffsets = {{
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
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagBrakesBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>, 2>
    kDiagBlockBrakesSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 4, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 0, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 4, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 1, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 5, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 1, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 2, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 3, SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BRAKES + 5, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesOffsets = {{
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
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 3 } }, { { 0, 0, 24 }, { 32, 32, 3 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBoosterSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_1 + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_2 + 0,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_1 + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIAG_BOOSTER_2 + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBoosterOffsets = {{
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
    kDiagBoosterBoundBoxes = {{
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 2,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 3,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 5,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 6,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 7,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 8,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 9,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 10,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 11,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 13,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 14,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 15,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 18,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 19,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 20,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 21,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 22,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_FLAT_TO_STEEP_DIAGONAL + 23,
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
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 0,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 1,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 2,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 3,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 4,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 5,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 6,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 7,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 8,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 9,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 10,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 11,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 13,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 14,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 15,
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
            { { 0, 6, 36 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, 48 }, { 32, 32, 1 } },
            {},
            {},
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 31, 0, 0 }, { 1, 32, 64 } },
            { { 6, 0, 36 }, { 20, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            { { 2, 2, 4 }, { 28, 28, 1 } },
            { { 0, 0, 0 }, { 32, 1, 64 } },
            { { 0, 6, 36 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 0, -8 }, { 32, 32, 1 } },
            {},
            {},
            { { 2, 2, 12 }, { 28, 28, 1 } },
            { { 6, 0, 58 }, { 20, 32, 1 } },
            { { 6, 0, 36 }, { 20, 32, 1 } },
        }},
    }};

    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kRightEighthDiveLoopUpToOrthogonalSprites = {{
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 16,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 17,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 18,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 19,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 20,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 21,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 22,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 23,
        }},
        {{
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 24,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 25,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 26,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 27,
        }},
        {{
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 28,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 29,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 30,
            SPR_G2_LATTICE_TRIANGLE_TRACK_DIVE_LOOP + 31,
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
        { &kEndStationSprites[0][0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
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
        { &kBrakesSprites[0][0], nullptr, &kBrakesBoundBoxes[0][0], false }, // Brakes
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
        { &kFlatToUp60LongBaseSprites[0][0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // Whirlpool
        { nullptr, nullptr, nullptr, true }, // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // FlatToDown60LongBase
        { &kCableLiftHillSprites[0][0][0], nullptr, &kCableLiftHillBoundBoxes[0][0][0], false }, // CableLiftHill
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
        { &kPoweredLiftSprites[0], nullptr, &kPoweredLiftBoundBoxes[0], false }, // PoweredLift
        { &kLeftLargeHalfLoopUpSprites[0][0], nullptr, &kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftLargeHalfLoopUp
        { &kRightLargeHalfLoopUpSprites[0][0], nullptr, &kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightLargeHalfLoopUp
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
        { &kUp90ToInvertedFlatQuarterLoopSprites[0][0], nullptr, &kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &kInvertedFlatToDown90QuarterLoopSprites[0][0], nullptr, &kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0], false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0][0], nullptr, &kBlockBrakesBoundBoxes[0][0], false }, // BlockBrakes
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
        { &kLeftLargeCorkscrewUpSprites[0][0], nullptr, &kLeftLargeCorkscrewUpBoundBoxes[0][0], false }, // LeftLargeCorkscrewUp
        { &kRightLargeCorkscrewUpSprites[0][0], nullptr, &kRightLargeCorkscrewUpBoundBoxes[0][0], false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { &kLeftMediumHalfLoopUpSprites[0][0], nullptr, &kLeftMediumHalfLoopUpBoundBoxes[0][0], false }, // LeftMediumHalfLoopUp
        { &kRightMediumHalfLoopUpSprites[0][0], nullptr, &kRightMediumHalfLoopUpBoundBoxes[0][0], false }, // RightMediumHalfLoopUp
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
        { &kDiagBrakesSprites[0][0][0][0], &kDiagBrakesOffsets[0][0][0], &kDiagBrakesBoundBoxes[0][0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0][0], &kDiagBlockBrakesOffsets[0][0][0], &kDiagBlockBrakesBoundBoxes[0][0][0], false }, // DiagBlockBrakes
        { nullptr, nullptr, nullptr, false }, // Down25Brakes
        { &kDiagBoosterSprites[0][0], &kDiagBoosterOffsets[0][0], &kDiagBoosterBoundBoxes[0][0], false }, // DiagBooster
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
} // namespace OpenRCT2::LatticeTriangle
