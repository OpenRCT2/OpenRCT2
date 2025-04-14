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

namespace OpenRCT2::MineTrainCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            20052,
            20053,
            20052,
            20053,
        }},
        {{
            20054,
            20055,
            20056,
            20057,
        }},
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kFlatBoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kEndStationSprites = {{
        {{
            20060,
            20061,
            20060,
            20061,
        }},
        {{
            20062,
            20063,
            20062,
            20063,
        }},
    }};
    // clang-format on

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kStationSprites = {{
        20064,
        20065,
        20064,
        20065,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kStationBoundBoxes = { {
        { { 0, 6, 1 }, { 32, 20, 1 } },
        { { 6, 0, 1 }, { 20, 32, 1 } },
        { { 0, 6, 1 }, { 32, 20, 1 } },
        { { 6, 0, 1 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            20074,
            20075,
            20076,
            20077,
        }},
        {{
            20102,
            20103,
            20104,
            20105,
        }},
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kUp25BoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp60Sprites = {{
        20090,
        20091,
        20092,
        20093,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kUp60BoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 0, 27, 0 }, { 32, 1, 98 } },
        { { 27, 0, 0 }, { 1, 32, 98 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            20066,
            20067,
            20068,
            20069,
        }},
        {{
            20094,
            20095,
            20096,
            20097,
        }},
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kFlatToUp25BoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ 20078, kImageIndexUndefined, }},
        {{ 20079, 20082, }},
        {{ 20080, 20083, }},
        {{ 20081, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kUp25ToUp60BoundBoxes = { {
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 66 } } } },
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 66 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Sprites = {{
        {{ 20084, kImageIndexUndefined, }},
        {{ 20085, 20088, }},
        {{ 20086, 20089, }},
        {{ 20087, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kUp60ToUp25BoundBoxes = { {
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
        { { { { 29, 4, -16 }, { 1, 24, 61 } }, { { 4, 0, 0 }, { 2, 32, 66 } } } },
        { { { { 4, 29, -16 }, { 24, 1, 61 } }, { { 0, 4, 0 }, { 32, 2, 66 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            20070,
            20071,
            20072,
            20073,
        }},
        {{
            20098,
            20099,
            20100,
            20101,
        }},
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kUp25ToFlatBoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            20155,
            kImageIndexUndefined,
            20154,
            20153,
            kImageIndexUndefined,
            20152,
            20151,
        }},
        {{
            20160,
            kImageIndexUndefined,
            20159,
            20158,
            kImageIndexUndefined,
            20157,
            20156,
        }},
        {{
            20165,
            kImageIndexUndefined,
            20164,
            20163,
            kImageIndexUndefined,
            20162,
            20161,
        }},
        {{
            20150,
            kImageIndexUndefined,
            20149,
            20148,
            kImageIndexUndefined,
            20147,
            20146,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections> kLeftQuarterTurn5TilesBoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 16, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        {{ 20106, 20114, }},
        {{ 20107, 20115, }},
        {{ 20108, kImageIndexUndefined, }},
        {{ 20109, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kFlatToLeftBankBoundBoxes = { {
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        {{ 20110, kImageIndexUndefined, }},
        {{ 20111, kImageIndexUndefined, }},
        {{ 20112, 20116, }},
        {{ 20113, 20117, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
        kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 20175, 20186, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20174, kImageIndexUndefined, }},
            {{ 20173, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20172, kImageIndexUndefined, }},
            {{ 20171, kImageIndexUndefined, }},
        }},
        {{
            {{ 20180, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20179, kImageIndexUndefined, }},
            {{ 20178, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20177, kImageIndexUndefined, }},
            {{ 20176, kImageIndexUndefined, }},
        }},
        {{
            {{ 20185, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20184, kImageIndexUndefined, }},
            {{ 20183, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20182, kImageIndexUndefined, }},
            {{ 20181, 20187, }},
        }},
        {{
            {{ 20170, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20169, kImageIndexUndefined, }},
            {{ 20168, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20167, kImageIndexUndefined, }},
            {{ 20166, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
        kBankedLeftQuarterTurn5TilesBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
                { { {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
            } },
            { {
                { { { { 27, 0, 0 }, { 1, 32, 26 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 16, 32, 1 } }, {} } },
                { { { { 16, 16, 27 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 27, 0 }, { 32, 1, 26 } }, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        {{ 20118, 20122, }},
        {{ 20119, 20123, }},
        {{ 20120, kImageIndexUndefined, }},
        {{ 20121, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kLeftBankToUp25BoundBoxes = { {
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 34 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 34 } } } },
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        {{ 20124, kImageIndexUndefined, }},
        {{ 20125, kImageIndexUndefined, }},
        {{ 20126, 20128, }},
        {{ 20127, 20129, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
        kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 20130, 20134, }},
        {{ 20131, 20135, }},
        {{ 20132, kImageIndexUndefined, }},
        {{ 20133, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kUp25ToLeftBankBoundBoxes = { {
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 34 } } } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 34 } } } },
        { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
        { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        {{ 20136, kImageIndexUndefined, }},
        {{ 20137, kImageIndexUndefined, }},
        {{ 20138, 20140, }},
        {{ 20139, 20141, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
        kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        20142,
        20143,
        20144,
        20145,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kLeftBankBoundBoxes = { {
        { { 0, 27, 0 }, { 32, 1, 26 } },
        { { 27, 0, 0 }, { 1, 32, 26 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            20250,
            kImageIndexUndefined,
            20251,
            20252,
            kImageIndexUndefined,
            20253,
            20254,
        }},
        {{
            20255,
            kImageIndexUndefined,
            20256,
            20257,
            kImageIndexUndefined,
            20258,
            20259,
        }},
        {{
            20260,
            kImageIndexUndefined,
            20261,
            20262,
            kImageIndexUndefined,
            20263,
            20264,
        }},
        {{
            20265,
            kImageIndexUndefined,
            20266,
            20267,
            kImageIndexUndefined,
            20268,
            20269,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections> kLeftQuarterTurn5TilesUp25BoundBoxes = { {
        { {
            { { 0, 2, 0 }, { 32, 27, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 2, 0, 0 }, { 27, 32, 1 } },
        } },
        { {
            { { 2, 0, 0 }, { 27, 32, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 16, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 2, 0 }, { 32, 27, 1 } },
        } },
        { {
            { { 0, 2, 0 }, { 32, 27, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 2, 0, 0 }, { 27, 32, 1 } },
        } },
        { {
            { { 2, 0, 0 }, { 27, 32, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 0, 2, 0 }, { 32, 27, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            20230,
            kImageIndexUndefined,
            20231,
            20232,
            kImageIndexUndefined,
            20233,
            20234,
        }},
        {{
            20235,
            kImageIndexUndefined,
            20236,
            20237,
            kImageIndexUndefined,
            20238,
            20239,
        }},
        {{
            20240,
            kImageIndexUndefined,
            20241,
            20242,
            kImageIndexUndefined,
            20243,
            20244,
        }},
        {{
            20245,
            kImageIndexUndefined,
            20246,
            20247,
            kImageIndexUndefined,
            20248,
            20249,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
        kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            20270,
            20271,
            20272,
            20273,
        }},
        {{
            20274,
            20275,
            20276,
            20277,
        }},
        {{
            20273,
            20272,
            20271,
            20270,
        }},
        {{
            20277,
            20276,
            20275,
            20274,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kSBendLeftBoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 0, 0 }, { 32, 26, 1 } },
            { { 0, 6, 0 }, { 32, 26, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 0, 0, 0 }, { 26, 32, 1 } },
            { { 6, 0, 0 }, { 26, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 6, 0 }, { 32, 26, 1 } },
            { { 0, 0, 0 }, { 32, 26, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 6, 0, 0 }, { 26, 32, 1 } },
            { { 0, 0, 0 }, { 26, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            20278,
            20279,
            20280,
            20281,
        }},
        {{
            20282,
            20283,
            20284,
            20285,
        }},
        {{
            20281,
            20280,
            20279,
            20278,
        }},
        {{
            20285,
            20284,
            20283,
            20282,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            20193,
            kImageIndexUndefined,
            20192,
            20191,
        }},
        {{
            20196,
            kImageIndexUndefined,
            20195,
            20194,
        }},
        {{
            20199,
            kImageIndexUndefined,
            20198,
            20197,
        }},
        {{
            20190,
            kImageIndexUndefined,
            20189,
            20188,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftQuarterTurn3TilesBoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 16, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            {{ 20205, 20212, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20204, kImageIndexUndefined, }},
            {{ 20203, kImageIndexUndefined, }},
        }},
        {{
            {{ 20208, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20207, kImageIndexUndefined, }},
            {{ 20206, kImageIndexUndefined, }},
        }},
        {{
            {{ 20211, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20210, kImageIndexUndefined, }},
            {{ 20209, 20213, }},
        }},
        {{
            {{ 20202, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20201, kImageIndexUndefined, }},
            {{ 20200, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kLeftBankedQuarterTurn3TilesBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
            } },
            { {
                { { { { 27, 0, 0 }, { 1, 32, 26 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 16, 16, 1 } }, {} } },
                { { { { 0, 27, 0 }, { 32, 1, 26 } }, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            20225,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20224,
        }},
        {{
            20227,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20226,
        }},
        {{
            20229,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20228,
        }},
        {{
            20223,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20222,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kLeftQuarterTurn3TilesUp25Offsets = { {
        { {
            { 0, 6, 0 },
            {},
            {},
            { 6, 0, 0 },
        } },
        { {
            { 6, 0, 0 },
            {},
            {},
            { 0, 6, 0 },
        } },
        { {
            { 0, 6, 0 },
            {},
            {},
            { 6, 0, 0 },
        } },
        { {
            { 6, 0, 0 },
            {},
            {},
            { 0, 6, 0 },
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftQuarterTurn3TilesUp25BoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
        { {
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 1 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            20214,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20215,
        }},
        {{
            20216,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20217,
        }},
        {{
            20218,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20219,
        }},
        {{
            20220,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20221,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kRightQuarterTurn3TilesUp25Offsets = { {
        { {
            { 0, 6, 0 },
            {},
            {},
            { 6, 0, 0 },
        } },
        { {
            { 6, 0, 0 },
            {},
            {},
            { 0, 6, 0 },
        } },
        { {
            { 0, 6, 0 },
            {},
            {},
            { 6, 0, 0 },
        } },
        { {
            { 6, 0, 0 },
            {},
            {},
            { 0, 6, 0 },
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn3TilesUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 20305, 20312, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20304, kImageIndexUndefined, }},
            {{ 20303, kImageIndexUndefined, }},
            {{ 20302, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20301, kImageIndexUndefined, }},
            {{ 20300, kImageIndexUndefined, }},
        }},
        {{
            {{ 20308, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20307, kImageIndexUndefined, }},
            {{ 20306, kImageIndexUndefined, }},
            {{ 20305, 20312, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20304, kImageIndexUndefined, }},
            {{ 20303, kImageIndexUndefined, }},
        }},
        {{
            {{ 20311, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20310, kImageIndexUndefined, }},
            {{ 20309, 20313, }},
            {{ 20308, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20307, kImageIndexUndefined, }},
            {{ 20306, kImageIndexUndefined, }},
        }},
        {{
            {{ 20302, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20301, kImageIndexUndefined, }},
            {{ 20300, kImageIndexUndefined, }},
            {{ 20311, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20310, kImageIndexUndefined, }},
            {{ 20309, 20313, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
        kLeftHalfBankedHelixUpSmallBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 0, 6, 8 }, { 32, 20, 1 } }, {} } },
            } },
            { {
                { { { { 27, 0, 0 }, { 1, 32, 26 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 16, 16, 1 } }, {} } },
                { { { { 0, 27, 0 }, { 32, 1, 26 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
                { { { { 27, 0, 0 }, { 1, 32, 26 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 16, 16, 1 } }, {} } },
                { { { { 0, 27, 0 }, { 32, 1, 26 } }, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 0, 6, 8 }, { 32, 20, 1 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 20286, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20287, kImageIndexUndefined, }},
            {{ 20288, kImageIndexUndefined, }},
            {{ 20289, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20290, kImageIndexUndefined, }},
            {{ 20291, 20298, }},
        }},
        {{
            {{ 20289, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20290, kImageIndexUndefined, }},
            {{ 20291, 20298, }},
            {{ 20292, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20293, kImageIndexUndefined, }},
            {{ 20294, kImageIndexUndefined, }},
        }},
        {{
            {{ 20292, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20293, kImageIndexUndefined, }},
            {{ 20294, kImageIndexUndefined, }},
            {{ 20295, 20299, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20296, kImageIndexUndefined, }},
            {{ 20297, kImageIndexUndefined, }},
        }},
        {{
            {{ 20295, 20299, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20296, kImageIndexUndefined, }},
            {{ 20297, kImageIndexUndefined, }},
            {{ 20286, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20287, kImageIndexUndefined, }},
            {{ 20288, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
        kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 20345, 20356, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20344, kImageIndexUndefined, }},
            {{ 20343, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20342, kImageIndexUndefined, }},
            {{ 20341, kImageIndexUndefined, }},
            {{ 20340, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20339, kImageIndexUndefined, }},
            {{ 20338, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20337, kImageIndexUndefined, }},
            {{ 20336, kImageIndexUndefined, }},
        }},
        {{
            {{ 20350, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20349, kImageIndexUndefined, }},
            {{ 20348, 20178, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20347, 20177, }},
            {{ 20346, kImageIndexUndefined, }},
            {{ 20345, 20356, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20344, kImageIndexUndefined, }},
            {{ 20343, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20342, kImageIndexUndefined, }},
            {{ 20341, kImageIndexUndefined, }},
        }},
        {{
            {{ 20355, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20354, kImageIndexUndefined, }},
            {{ 20353, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20352, kImageIndexUndefined, }},
            {{ 20351, 20357, }},
            {{ 20350, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20349, kImageIndexUndefined, }},
            {{ 20348, 20178, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20347, 20177, }},
            {{ 20346, kImageIndexUndefined, }},
        }},
        {{
            {{ 20340, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20339, kImageIndexUndefined, }},
            {{ 20338, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20337, kImageIndexUndefined, }},
            {{ 20336, kImageIndexUndefined, }},
            {{ 20355, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20354, kImageIndexUndefined, }},
            {{ 20353, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20352, kImageIndexUndefined, }},
            {{ 20351, 20357, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
        kLeftHalfBankedHelixUpLargeBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
                { { {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 6, 8 }, { 32, 20, 1 } }, {} } },
            } },
            { {
                { { { { 27, 0, 0 }, { 1, 32, 26 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 16, 32, 1 } }, {} } },
                { { { { 16, 16, 27 }, { 16, 16, 1 } }, { { 16, 16, 27 }, { 16, 16, 1 } } } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 32, 16, 1 } }, { { 0, 0, 27 }, { 32, 16, 1 } } } },
                { { { { 0, 27, 0 }, { 32, 1, 26 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } } },
                { { {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
                { { { { 27, 0, 0 }, { 1, 32, 26 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 16, 32, 1 } }, {} } },
                { { { { 16, 16, 27 }, { 16, 16, 1 } }, { { 16, 16, 27 }, { 16, 16, 1 } } } },
                { { {}, {} } },
                { { { { 0, 0, 27 }, { 32, 16, 1 } }, { { 0, 0, 27 }, { 32, 16, 1 } } } },
                { { { { 0, 27, 0 }, { 32, 1, 26 } }, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, {} } },
                { { {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 0, 6, 8 }, { 32, 20, 1 } }, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 1 } }, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 1 } }, {} } },
                { { {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 1 } }, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 20314, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20315, kImageIndexUndefined, }},
            {{ 20316, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20317, kImageIndexUndefined, }},
            {{ 20318, kImageIndexUndefined, }},
            {{ 20319, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20320, kImageIndexUndefined, }},
            {{ 20321, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20322, kImageIndexUndefined, }},
            {{ 20323, 20334, }},
        }},
        {{
            {{ 20319, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20320, kImageIndexUndefined, }},
            {{ 20321, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20322, kImageIndexUndefined, }},
            {{ 20323, 20334, }},
            {{ 20324, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20325, kImageIndexUndefined, }},
            {{ 20326, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20327, kImageIndexUndefined, }},
            {{ 20328, kImageIndexUndefined, }},
        }},
        {{
            {{ 20324, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20325, kImageIndexUndefined, }},
            {{ 20326, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20327, kImageIndexUndefined, }},
            {{ 20328, kImageIndexUndefined, }},
            {{ 20329, 20335, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20330, kImageIndexUndefined, }},
            {{ 20331, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20332, kImageIndexUndefined, }},
            {{ 20333, kImageIndexUndefined, }},
        }},
        {{
            {{ 20329, 20335, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20330, kImageIndexUndefined, }},
            {{ 20331, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20332, kImageIndexUndefined, }},
            {{ 20333, kImageIndexUndefined, }},
            {{ 20314, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20315, kImageIndexUndefined, }},
            {{ 20316, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20317, kImageIndexUndefined, }},
            {{ 20318, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
        kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);

    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        20058,
        20059,
        20058,
        20059,
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kBrakesBoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 0,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 1,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 2,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 3,
        }},
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 4,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 5,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 6,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 7,
        }},
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 8,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 9,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 10,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 11,
        }},
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 12,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 13,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 14,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 15,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kFlatToUp60LongBaseBoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 4, 28, 0 }, { 24, 2, 56 } },
        } },
        { {
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 28, 4, 0 }, { 2, 24, 56 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp60ToFlatLongBaseSprites = {{
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 16,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 17,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 18,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 19,
        }},
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 20,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 21,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 22,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 23,
        }},
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 24,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 25,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 26,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 27,
        }},
        {{
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 28,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 29,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 30,
            SPR_G2_MINETRAIN_RC_FLAT_TO_STEEP + 31,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp60ToFlatLongBaseBoundBoxes = { {
        { {
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        } },
        { {
            { { 4, 28, 0 }, { 24, 2, 56 } },
            { { 4, 28, 0 }, { 24, 2, 56 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        } },
        { {
            { { 28, 4, 0 }, { 2, 24, 56 } },
            { { 28, 4, 0 }, { 2, 24, 56 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        } },
        { {
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            20452,
            20453,
            20454,
            kImageIndexUndefined,
            20455,
        }},
        {{
            20456,
            20457,
            20458,
            kImageIndexUndefined,
            20459,
        }},
        {{
            20460,
            20461,
            20462,
            kImageIndexUndefined,
            20463,
        }},
        {{
            20464,
            20465,
            20466,
            kImageIndexUndefined,
            20467,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections> kLeftEighthToDiagBoundBoxes = { {
        { {
            { { 0, 0, 0 }, { 32, 32, 1 } },
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 1 } },
        } },
        { {
            { { 0, 0, 0 }, { 32, 32, 1 } },
            { { 0, 0, 0 }, { 16, 34, 1 } },
            { { 16, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 0, 0 }, { 18, 16, 1 } },
        } },
        { {
            { { 0, 0, 0 }, { 32, 32, 1 } },
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
        } },
        { {
            { { 0, 0, 0 }, { 32, 32, 1 } },
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            20436,
            20437,
            20438,
            kImageIndexUndefined,
            20439,
        }},
        {{
            20440,
            20441,
            20442,
            kImageIndexUndefined,
            20443,
        }},
        {{
            20444,
            20445,
            20446,
            kImageIndexUndefined,
            20447,
        }},
        {{
            20448,
            20449,
            20450,
            kImageIndexUndefined,
            20451,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
        kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            20484,
            20485,
            20486,
            kImageIndexUndefined,
            20487,
        }},
        {{
            20488,
            20489,
            20490,
            kImageIndexUndefined,
            20491,
        }},
        {{
            20492,
            20493,
            20494,
            kImageIndexUndefined,
            20495,
        }},
        {{
            20496,
            20497,
            20498,
            kImageIndexUndefined,
            20499,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections> kLeftEighthBankToDiagBoundBoxes = { {
        { {
            { { 0, 27, 0 }, { 32, 1, 26 } },
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 1 } },
        } },
        { {
            { { 27, 0, 0 }, { 1, 32, 26 } },
            { { 0, 0, 27 }, { 16, 34, 0 } },
            { { 16, 16, 27 }, { 16, 16, 0 } },
            {},
            { { 16, 0, 27 }, { 18, 16, 0 } },
        } },
        { {
            { { 0, 0, 0 }, { 32, 32, 1 } },
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
        } },
        { {
            { { 0, 0, 0 }, { 32, 32, 1 } },
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 1 } },
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            20468,
            20469,
            20470,
            kImageIndexUndefined,
            20471,
        }},
        {{
            20472,
            20473,
            20474,
            kImageIndexUndefined,
            20475,
        }},
        {{
            20476,
            20477,
            20478,
            kImageIndexUndefined,
            20479,
        }},
        {{
            20480,
            20481,
            20482,
            kImageIndexUndefined,
            20483,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
        kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                20358,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20359,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20360,
                kImageIndexUndefined,
            }},
            {{
                20361,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                20386,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20387,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20388,
                kImageIndexUndefined,
            }},
            {{
                20389,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagFlatOffsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagFlatBoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                20370,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20371,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20372,
                kImageIndexUndefined,
            }},
            {{
                20373,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                20398,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20399,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20400,
                kImageIndexUndefined,
            }},
            {{
                20401,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagUp25Offsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagUp25BoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            kImageIndexUndefined,
            20382,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20383,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            20384,
            kImageIndexUndefined,
        }},
        {{
            20385,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagUp60Offsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagUp60BoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                20362,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20363,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20364,
                kImageIndexUndefined,
            }},
            {{
                20365,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                20390,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20391,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20392,
                kImageIndexUndefined,
            }},
            {{
                20393,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagFlatToUp25Offsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagFlatToUp25BoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            kImageIndexUndefined,
            20374,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20375,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            20376,
            kImageIndexUndefined,
        }},
        {{
            20377,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagUp25ToUp60Offsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagUp25ToUp60BoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            20378,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20379,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            20380,
            kImageIndexUndefined,
        }},
        {{
            20381,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagUp60ToUp25Offsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagUp60ToUp25BoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25ToFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                20366,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20367,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20368,
                kImageIndexUndefined,
            }},
            {{
                20369,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                20394,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                20395,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                20396,
                kImageIndexUndefined,
            }},
            {{
                20397,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagUp25ToFlatOffsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagUp25ToFlatBoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20406, 20410, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20407, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20408, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 20409, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagFlatToLeftBankOffsets = { {
            { {
                { { {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagFlatToLeftBankBoundBoxes = { {
            { {
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20411, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20412, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20413, 20415, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 20414, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagFlatToRightBankOffsets = { {
            { {
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagFlatToRightBankBoundBoxes = { {
            { {
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20426, 20430, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20427, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20428, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 20429, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagLeftBankToUp25Offsets = { {
            { {
                { { {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagLeftBankToUp25BoundBoxes = { {
            { {
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20431, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20432, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20433, 20435, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 20434, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagRightBankToUp25Offsets = { {
            { {
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagRightBankToUp25BoundBoxes = { {
            { {
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20416, 20420, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20417, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20418, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 20419, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToLeftBankOffsets = { {
            { {
                { { {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToLeftBankBoundBoxes = { {
            { {
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20421, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20422, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 20423, 20425, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 20424, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToRightBankOffsets = { {
            { {
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToRightBankBoundBoxes = { {
            { {
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
            } },
            { {
                { { {}, {} } },
                { { {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 1 } }, {} } },
                { { {}, {} } },
                { { {}, {} } },
                { { {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            20402,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            20403,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            20404,
            kImageIndexUndefined,
        }},
        {{
            20405,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagLeftBankOffsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagLeftBankBoundBoxes = { {
        { {
            {},
            { { -16, -16, 27 }, { 32, 32, 0 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 3 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            20060,
            20061,
            20060,
            20061,
        }},
        {{
            20062,
            20063,
            20062,
            20063,
        }},
    }};
    // clang-format on

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kBlockBrakesBoundBoxes = { {
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_MINETRAIN_DIAG_BRAKES,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_MINETRAIN_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_MINETRAIN_DIAG_BRAKES,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_MINETRAIN_DIAG_BRAKES + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagBrakesOffsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagBrakesBoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagBlockBrakesSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_MINETRAIN_DIAG_BRAKES + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MINETRAIN_DIAG_BRAKES + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MINETRAIN_DIAG_BRAKES + 3,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_MINETRAIN_DIAG_BRAKES + 5,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_MINETRAIN_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MINETRAIN_DIAG_BRAKES + 4,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_MINETRAIN_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_MINETRAIN_DIAG_BRAKES + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections> kDiagBlockBrakesOffsets = { {
        { {
            {},
            { -16, -16, 0 },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { -16, -16, 0 },
        } },
        { {
            {},
            {},
            { -16, -16, 0 },
            {},
        } },
        { {
            { -16, -16, 0 },
            {},
            {},
            {},
        } },
    } };

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kDiagBlockBrakesBoundBoxes = { {
        { {
            {},
            { { -16, -16, 0 }, { 32, 32, 13 } },
            {},
            {},
        } },
        { {
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 13 } },
        } },
        { {
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 13 } },
            {},
        } },
        { {
            { { -16, -16, 0 }, { 32, 32, 13 } },
            {},
            {},
            {},
        } },
    } };

    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false },                // Flat
        { &kEndStationSprites[0][0], nullptr, &kStationBoundBoxes[0], false },       // EndStation
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false },             // BeginStation
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false },             // MiddleStation
        { &kUp25Sprites[0][0], nullptr, &kUp25BoundBoxes[0], false },                // Up25
        { &kUp60Sprites[0], nullptr, &kUp60BoundBoxes[0], false },                   // Up60
        { &kFlatToUp25Sprites[0][0], nullptr, &kFlatToUp25BoundBoxes[0], false },    // FlatToUp25
        { &kUp25ToUp60Sprites[0][0], nullptr, &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0], nullptr, &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], nullptr, &kUp25ToFlatBoundBoxes[0], false },    // Up25ToFlat
        { nullptr, nullptr, nullptr, true },                                         // Down25
        { nullptr, nullptr, nullptr, true },                                         // Down60
        { nullptr, nullptr, nullptr, true },                                         // FlatToDown25
        { nullptr, nullptr, nullptr, true },                                         // Down25ToDown60
        { nullptr, nullptr, nullptr, true },                                         // Down60ToDown25
        { nullptr, nullptr, nullptr, true },                                         // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0], nullptr, &kLeftQuarterTurn5TilesBoundBoxes[0][0],
          false },                                                                             // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true },                                                   // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0][0], nullptr, &kFlatToLeftBankBoundBoxes[0][0], false },   // FlatToLeftBank
        { &kFlatToRightBankSprites[0][0], nullptr, &kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true },                                                   // LeftBankToFlat
        { nullptr, nullptr, nullptr, true },                                                   // RightBankToFlat
        { &kBankedLeftQuarterTurn5TilesSprites[0][0][0], nullptr, &kBankedLeftQuarterTurn5TilesBoundBoxes[0][0][0],
          false },                                                                             // BankedLeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true },                                                   // BankedRightQuarterTurn5Tiles
        { &kLeftBankToUp25Sprites[0][0], nullptr, &kLeftBankToUp25BoundBoxes[0][0], false },   // LeftBankToUp25
        { &kRightBankToUp25Sprites[0][0], nullptr, &kRightBankToUp25BoundBoxes[0][0], false }, // RightBankToUp25
        { &kUp25ToLeftBankSprites[0][0], nullptr, &kUp25ToLeftBankBoundBoxes[0][0], false },   // Up25ToLeftBank
        { &kUp25ToRightBankSprites[0][0], nullptr, &kUp25ToRightBankBoundBoxes[0][0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true },                                                   // LeftBankToDown25
        { nullptr, nullptr, nullptr, true },                                                   // RightBankToDown25
        { nullptr, nullptr, nullptr, true },                                                   // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true },                                                   // Down25ToRightBank
        { &kLeftBankSprites[0], nullptr, &kLeftBankBoundBoxes[0], false },                     // LeftBank
        { nullptr, nullptr, nullptr, true },                                                   // RightBank
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0], nullptr, &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0],
          false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0], nullptr, &kRightQuarterTurn5TilesUp25BoundBoxes[0][0],
          false },                                                                   // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true },                                         // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true },                                         // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], nullptr, &kSBendLeftBoundBoxes[0][0], false },   // SBendLeft
        { &kSBendRightSprites[0][0], nullptr, &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false },                                        // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false },                                        // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0], nullptr, &kLeftQuarterTurn3TilesBoundBoxes[0][0],
          false },                           // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0],
          false },                           // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &kLeftQuarterTurn3TilesUp25Sprites[0][0], &kLeftQuarterTurn3TilesUp25Offsets[0][0],
          &kLeftQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0], &kRightQuarterTurn3TilesUp25Offsets[0][0],
          &kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true },                     // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true },                     // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false },                    // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true },                     // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false },                    // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false },                    // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true },                     // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true },                     // RightTwistUpToDown
        { nullptr, nullptr, nullptr, false },                    // HalfLoopUp
        { nullptr, nullptr, nullptr, true },                     // HalfLoopDown
        { nullptr, nullptr, nullptr, false },                    // LeftCorkscrewUp
        { nullptr, nullptr, nullptr, false },                    // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true },                     // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true },                     // RightCorkscrewDown
        { nullptr, nullptr, nullptr, false },                    // FlatToUp60
        { nullptr, nullptr, nullptr, false },                    // Up60ToFlat
        { nullptr, nullptr, nullptr, true },                     // FlatToDown60
        { nullptr, nullptr, nullptr, true },                     // Down60ToFlat
        { nullptr, nullptr, nullptr, false },                    // TowerBase
        { nullptr, nullptr, nullptr, false },                    // TowerSection
        { nullptr, nullptr, nullptr, false },                    // FlatCovered
        { nullptr, nullptr, nullptr, false },                    // Up25Covered
        { nullptr, nullptr, nullptr, false },                    // Up60Covered
        { nullptr, nullptr, nullptr, false },                    // FlatToUp25Covered
        { nullptr, nullptr, nullptr, false },                    // Up25ToUp60Covered
        { nullptr, nullptr, nullptr, false },                    // Up60ToUp25Covered
        { nullptr, nullptr, nullptr, false },                    // Up25ToFlatCovered
        { nullptr, nullptr, nullptr, false },                    // Down25Covered
        { nullptr, nullptr, nullptr, false },                    // Down60Covered
        { nullptr, nullptr, nullptr, false },                    // FlatToDown25Covered
        { nullptr, nullptr, nullptr, false },                    // Down25ToDown60Covered
        { nullptr, nullptr, nullptr, false },                    // Down60ToDown25Covered
        { nullptr, nullptr, nullptr, false },                    // Down25ToFlatCovered
        { nullptr, nullptr, nullptr, false },                    // LeftQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr, false },                    // RightQuarterTurn5TilesCovered
        { nullptr, nullptr, nullptr, false },                    // SBendLeftCovered
        { nullptr, nullptr, nullptr, false },                    // SBendRightCovered
        { nullptr, nullptr, nullptr, false },                    // LeftQuarterTurn3TilesCovered
        { nullptr, nullptr, nullptr, false },                    // RightQuarterTurn3TilesCovered
        { &kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0],
          false }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0],
          false },                           // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0],
          false }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0],
          false },                                                     // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true },                           // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true },                           // RightHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, false },                          // LeftQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, false },                          // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true },                           // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true },                           // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0], nullptr, &kBrakesBoundBoxes[0], false }, // Brakes
        { nullptr, nullptr, nullptr, false },                          // Booster
        { nullptr, nullptr, nullptr, false },                          // Maze
        { nullptr, nullptr, nullptr, false },                          // LeftQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, false },                          // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, true },                           // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, true },                           // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, false },                          // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, false },                          // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, true },                           // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, true },                           // RightQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, false },                          // Up25LeftBanked
        { nullptr, nullptr, nullptr, false },                          // Up25RightBanked
        { nullptr, nullptr, nullptr, false },                          // Waterfall
        { nullptr, nullptr, nullptr, false },                          // Rapids
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false },  // OnRidePhoto
        { nullptr, nullptr, nullptr, true },                           // Down25LeftBanked
        { nullptr, nullptr, nullptr, true },                           // Down25RightBanked
        { nullptr, nullptr, nullptr, false },                          // Watersplash
        { &kFlatToUp60LongBaseSprites[0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false },                                                        // Whirlpool
        { nullptr, nullptr, nullptr, true },                                                         // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true },                                                         // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false },                                                        // CableLiftHill
        { nullptr, nullptr, nullptr, false },                                                        // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false },                                                        // ReverseFreefallVertical
        { nullptr, nullptr, nullptr, false },                                                        // Up90
        { nullptr, nullptr, nullptr, true },                                                         // Down90
        { nullptr, nullptr, nullptr, false },                                                        // Up60ToUp90
        { nullptr, nullptr, nullptr, true },                                                         // Down90ToDown60
        { nullptr, nullptr, nullptr, false },                                                        // Up90ToUp60
        { nullptr, nullptr, nullptr, true },                                                         // Down60ToDown90
        { nullptr, nullptr, nullptr, false },                                                        // BrakeForDrop
        { &kLeftEighthToDiagSprites[0][0], nullptr, &kLeftEighthToDiagBoundBoxes[0][0], false },     // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0], nullptr, &kRightEighthToDiagBoundBoxes[0][0], false },   // RightEighthToDiag
        { nullptr, nullptr, nullptr, true },                                                         // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true },                                                         // RightEighthToOrthogonal
        { &kLeftEighthBankToDiagSprites[0][0], nullptr, &kLeftEighthBankToDiagBoundBoxes[0][0], false }, // LeftEighthBankToDiag
        { &kRightEighthBankToDiagSprites[0][0], nullptr, &kRightEighthBankToDiagBoundBoxes[0][0],
          false },                           // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0], &kDiagFlatOffsets[0][0], &kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0], &kDiagUp25Offsets[0][0], &kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { &kDiagUp60Sprites[0][0], &kDiagUp60Offsets[0][0], &kDiagUp60BoundBoxes[0][0], false },    // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0],
          false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0], &kDiagUp25ToUp60Offsets[0][0], &kDiagUp25ToUp60BoundBoxes[0][0],
          false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0], &kDiagUp60ToUp25Offsets[0][0], &kDiagUp60ToUp25BoundBoxes[0][0],
          false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0], &kDiagUp25ToFlatOffsets[0][0], &kDiagUp25ToFlatBoundBoxes[0][0],
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
        { &kDiagLeftBankSprites[0][0], &kDiagLeftBankOffsets[0][0], &kDiagLeftBankBoundBoxes[0][0], false }, // DiagLeftBank
        { nullptr, nullptr, nullptr, true },                                                                 // DiagRightBank
        { nullptr, nullptr, nullptr, false },                                                                // LogFlumeReverser
        { nullptr, nullptr, nullptr, false },                                                                // SpinningTunnel
        { nullptr, nullptr, nullptr, false },                                       // LeftBarrelRollUpToDown
        { nullptr, nullptr, nullptr, false },                                       // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true },                                        // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true },                                        // RightBarrelRollDownToUp
        { nullptr, nullptr, nullptr, false },                                       // LeftBankToLeftQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, false },                                       // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true },                                        // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false },                                       // PoweredLift
        { nullptr, nullptr, nullptr, false },                                       // LeftLargeHalfLoopUp
        { nullptr, nullptr, nullptr, false },                                       // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr, true },                                        // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr, true },                                        // RightLargeHalfLoopDown
        { nullptr, nullptr, nullptr, false },                                       // LeftFlyerTwistUp
        { nullptr, nullptr, nullptr, false },                                       // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, true },                                        // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, true },                                        // RightFlyerTwistDown
        { nullptr, nullptr, nullptr, false },                                       // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false },                                       // FlyerHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, true },                                        // LeftFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, true },                                        // RightFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, false },                                       // LeftFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false },                                       // RightFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false },                                       // HeartLineTransferUp
        { nullptr, nullptr, nullptr, false },                                       // HeartLineTransferDown
        { nullptr, nullptr, nullptr, false },                                       // LeftHeartLineRoll
        { nullptr, nullptr, nullptr, false },                                       // RightHeartLineRoll
        { nullptr, nullptr, nullptr, false },                                       // MinigolfHoleA
        { nullptr, nullptr, nullptr, false },                                       // MinigolfHoleB
        { nullptr, nullptr, nullptr, false },                                       // MinigolfHoleC
        { nullptr, nullptr, nullptr, false },                                       // MinigolfHoleD
        { nullptr, nullptr, nullptr, false },                                       // MinigolfHoleE
        { nullptr, nullptr, nullptr, false },                                       // MultiDimInvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false },                                       // Up90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr, false },                                       // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false },                                       // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false },                                       // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false },                                       // LeftReverser
        { nullptr, nullptr, nullptr, true },                                        // RightReverser
        { nullptr, nullptr, nullptr, false },                                       // AirThrustTopCap
        { nullptr, nullptr, nullptr, true },                                        // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true },                                        // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0], nullptr, &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
        { nullptr, nullptr, nullptr, false },                                       // LeftBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, false },                                       // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true },                                        // RightBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, false },                                       // LeftBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, false },                                       // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true },                                        // RightBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, false },                                       // Up25ToLeftBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // Up25ToRightBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // LeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr, false },                                       // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true },                                        // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true },                                        // RightBankedDown25ToDown25
        { nullptr, nullptr, nullptr, false },                                       // LeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // RightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // LeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, false },                                       // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true },                                        // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true },                                        // RightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr, false },                                       // FlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // FlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // LeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr, false },                                       // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true },                                        // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true },                                        // RightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false },                                       // LeftQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, false },                                       // RightQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, true },                                        // LeftQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr, true },                                        // RightQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr, false },                                       // MultiDimUp90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr, false },                                       // MultiDimFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false },                                       // MultiDimInvertedUp90ToFlatQuarterLoop
        { nullptr, nullptr, nullptr, false },                                       // RotationControlToggle
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack1x4A
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack2x2
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack4x4
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack2x4
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack1x5
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack1x1A
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack1x4B
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack1x1B
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack1x4C
        { nullptr, nullptr, nullptr, false },                                       // FlatTrack3x3
        { nullptr, nullptr, nullptr, false },                                       // LeftLargeCorkscrewUp
        { nullptr, nullptr, nullptr, false },                                       // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true },                                        // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true },                                        // RightLargeCorkscrewDown
        { nullptr, nullptr, nullptr, false },                                       // LeftMediumHalfLoopUp
        { nullptr, nullptr, nullptr, false },                                       // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true },                                        // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true },                                        // RightMediumHalfLoopDown
        { nullptr, nullptr, nullptr, false },                                       // LeftZeroGRollUp
        { nullptr, nullptr, nullptr, false },                                       // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true },                                        // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true },                                        // RightZeroGRollDown
        { nullptr, nullptr, nullptr, false },                                       // LeftLargeZeroGRollUp
        { nullptr, nullptr, nullptr, false },                                       // RightLargeZeroGRollUp
        { nullptr, nullptr, nullptr, true },                                        // LeftLargeZeroGRollDown
        { nullptr, nullptr, nullptr, true },                                        // RightLargeZeroGRollDown
        { nullptr, nullptr, nullptr, false },                                       // LeftFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false },                                       // RightFlyerLargeHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false },                                       // LeftFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false },                                       // RightFlyerLargeHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, false },                                       // LeftFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false },                                       // RightFlyerLargeHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false },                                       // LeftFlyerLargeHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr, false },                                       // RightFlyerLargeHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr, false },                                       // FlyerHalfLoopInvertedUp
        { nullptr, nullptr, nullptr, false },                                       // FlyerHalfLoopUninvertedDown
        { nullptr, nullptr, nullptr, false },                                       // LeftEighthToDiagUp25
        { nullptr, nullptr, nullptr, false },                                       // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true },                                        // RightEighthToDiagDown25
        { nullptr, nullptr, nullptr, false },                                       // LeftEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, false },                                       // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, true },                                        // RightEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, false },                                       // DiagUp25ToLeftBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagUp25ToRightBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagLeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true },                                        // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagRightBankedDown25ToDown25
        { nullptr, nullptr, nullptr, false },                                       // DiagLeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagRightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagLeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, false },                                       // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true },                                        // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true },                                        // DiagRightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr, false },                                       // DiagFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false },                                       // DiagLeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr, false },                                       // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true },                                        // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                        // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true },                                        // DiagRightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false },                                       // DiagUp25LeftBanked
        { nullptr, nullptr, nullptr, false },                                       // DiagUp25RightBanked
        { nullptr, nullptr, nullptr, true },                                        // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr, true },                                        // DiagDown25RightBanked
        { nullptr, nullptr, nullptr, false },                                       // LeftEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, false },                                       // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, true },                                        // RightEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, false },                                       // LeftEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, false },                                       // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, true },                                        // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, true },                                        // RightEighthBankToOrthogonalDown25
        { &kDiagBrakesSprites[0][0], &kDiagBrakesOffsets[0][0], &kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0], &kDiagBlockBrakesOffsets[0][0], &kDiagBlockBrakesBoundBoxes[0][0],
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

} // namespace OpenRCT2::MineTrainCoaster
