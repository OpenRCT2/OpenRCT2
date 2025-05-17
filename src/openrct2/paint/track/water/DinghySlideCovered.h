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

namespace OpenRCT2::DinghySlideCovered
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatSprites = {{
        {{ 19736, 19738, }},
        {{ 19737, 19739, }},
        {{ 19736, 19738, }},
        {{ 19737, 19739, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25Sprites = {{
        {{ 19828, 19832, }},
        {{ 19829, 19833, }},
        {{ 19830, 19834, }},
        {{ 19831, 19835, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60Sprites = {{
        {{ 19852, 19856, }},
        {{ 19853, 19857, }},
        {{ 19854, 19858, }},
        {{ 19855, 19859, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 98 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 98 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 98 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 98 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        {{ 19812, 19816, }},
        {{ 19813, 19817, }},
        {{ 19814, 19818, }},
        {{ 19815, 19819, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ 19836, 19840, }},
        {{ 19837, 19841, }},
        {{ 19838, 19842, }},
        {{ 19839, 19843, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Sprites = {{
        {{ 19844, 19848, }},
        {{ 19845, 19849, }},
        {{ 19846, 19850, }},
        {{ 19847, 19851, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        {{ 19820, 19824, }},
        {{ 19821, 19825, }},
        {{ 19822, 19826, }},
        {{ 19823, 19827, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesSprites = {{
        {{
            {{ 19948, 19968, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19949, 19969, }},
            {{ 19950, 19970, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19951, 19971, }},
            {{ 19952, 19972, }},
        }},
        {{
            {{ 19953, 19973, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19954, 19974, }},
            {{ 19955, 19975, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19956, 19976, }},
            {{ 19957, 19977, }},
        }},
        {{
            {{ 19958, 19978, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19959, 19979, }},
            {{ 19960, 19980, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19961, 19981, }},
            {{ 19962, 19982, }},
        }},
        {{
            {{ 19963, 19983, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19964, 19984, }},
            {{ 19965, 19985, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19966, 19986, }},
            {{ 19967, 19987, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 30, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 34, 2 } }, { { 16, 0, 27 }, { 16, 34, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            {{ 20020, 20036, }},
            {{ 20021, 20037, }},
            {{ 20022, 20038, }},
            {{ 20023, 20039, }},
        }},
        {{
            {{ 20024, 20040, }},
            {{ 20025, 20041, }},
            {{ 20026, 20042, }},
            {{ 20027, 20043, }},
        }},
        {{
            {{ 20023, 20039, }},
            {{ 20022, 20038, }},
            {{ 20021, 20037, }},
            {{ 20020, 20036, }},
        }},
        {{
            {{ 20027, 20043, }},
            {{ 20026, 20042, }},
            {{ 20025, 20041, }},
            {{ 20024, 20040, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 26, 2 } }, { { 0, 0, 27 }, { 32, 26, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 26, 2 } }, { { 0, 6, 27 }, { 32, 26, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 26, 32, 2 } }, { { 0, 0, 27 }, { 26, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 26, 32, 2 } }, { { 6, 0, 27 }, { 26, 32, 0 } } }},
            {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
        }},
        {{
            {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 26, 2 } }, { { 0, 6, 27 }, { 32, 26, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 26, 2 } }, { { 0, 0, 27 }, { 32, 26, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 26, 32, 2 } }, { { 6, 0, 27 }, { 26, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 26, 32, 2 } }, { { 0, 0, 27 }, { 26, 32, 0 } } }},
            {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            {{ 20028, 20044, }},
            {{ 20029, 20045, }},
            {{ 20030, 20046, }},
            {{ 20031, 20047, }},
        }},
        {{
            {{ 20032, 20048, }},
            {{ 20033, 20049, }},
            {{ 20034, 20050, }},
            {{ 20035, 20051, }},
        }},
        {{
            {{ 20031, 20047, }},
            {{ 20030, 20046, }},
            {{ 20029, 20045, }},
            {{ 20028, 20044, }},
        }},
        {{
            {{ 20035, 20051, }},
            {{ 20034, 20050, }},
            {{ 20033, 20049, }},
            {{ 20032, 20048, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesSprites = {{
        {{
            {{ 19884, 19896, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19885, 19897, }},
            {{ 19886, 19898, }},
        }},
        {{
            {{ 19887, 19899, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19888, 19900, }},
            {{ 19889, 19901, }},
        }},
        {{
            {{ 19890, 19902, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19891, 19903, }},
            {{ 19892, 19904, }},
        }},
        {{
            {{ 19893, 19905, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 19894, 19906, }},
            {{ 19895, 19907, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // Flat
        { nullptr, nullptr, nullptr, false }, // EndStation
        { nullptr, nullptr, nullptr, false }, // BeginStation
        { nullptr, nullptr, nullptr, false }, // MiddleStation
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
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5Tiles
        { &kRightQuarterTurn5TilesSprites[0][0][0], nullptr, &kRightQuarterTurn5TilesBoundBoxes[0][0][0], false }, // RightQuarterTurn5Tiles
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
        { &kSBendLeftSprites[0][0][0], nullptr, &kSBendLeftBoundBoxes[0][0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0][0], nullptr, &kSBendRightBoundBoxes[0][0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3Tiles
        { &kRightQuarterTurn3TilesSprites[0][0][0], nullptr, &kRightQuarterTurn3TilesBoundBoxes[0][0][0], false }, // RightQuarterTurn3Tiles
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
        { nullptr, nullptr, nullptr, false }, // LeftEighthToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, false }, // DiagFlat
        { nullptr, nullptr, nullptr, false }, // DiagUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp60
        { nullptr, nullptr, nullptr, false }, // DiagFlatToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToUp60
        { nullptr, nullptr, nullptr, false }, // DiagUp60ToUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToFlat
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
} // namespace OpenRCT2::DinghySlideCovered
