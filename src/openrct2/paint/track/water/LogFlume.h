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

namespace OpenRCT2::LogFlume
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatSprites = {{
        {{ 20996, 21000, }},
        {{ 20997, 21001, }},
        {{ 20998, 21002, }},
        {{ 20999, 21003, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            {{ 20996, kImageIndexUndefined, }},
            {{ 20997, kImageIndexUndefined, }},
            {{ 20998, kImageIndexUndefined, }},
            {{ 20999, kImageIndexUndefined, }},
        }},
        {{
            {{ 20996, 21000, }},
            {{ 20997, 21001, }},
            {{ 20998, 21002, }},
            {{ 20999, 21003, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        {{ { { 0, 6, 1 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 1 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 1 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 1 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25Sprites = {{
        {{ 21028, 21032, }},
        {{ 21029, 21033, }},
        {{ 21030, 21034, }},
        {{ 21031, 21035, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp25Sprites = {{
        {{ 21012, 21016, }},
        {{ 21013, 21017, }},
        {{ 21014, 21018, }},
        {{ 21015, 21019, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToFlatSprites = {{
        {{ 21020, 21024, }},
        {{ 21021, 21025, }},
        {{ 21022, 21026, }},
        {{ 21023, 21027, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown25Sprites = {{
        {{ 21046, 21034, }},
        {{ 21047, 21035, }},
        {{ 21044, 21032, }},
        {{ 21045, 21033, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60Sprites = {{
        {{ SPR_G2_FLUME_60_NW_SE_BACK + 0, SPR_G2_EMPTY + 0, }},
        {{ SPR_G2_EMPTY + 0, SPR_G2_FLUME_60_NW_SE + 0, }},
        {{ SPR_G2_EMPTY + 0, SPR_G2_FLUME_60_NE_SW + 0, }},
        {{ SPR_G2_FLUME_60_NE_SW_BACK + 0, SPR_G2_EMPTY + 0, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60BoundBoxes = {{
        {{ { { 0, 27, 4 }, { 32, 1, 98 } }, { { 0, 6, 0 }, { 32, 20, 2 } } }},
        {{ { { 27, 0, 4 }, { 1, 32, 98 } }, { { 6, 0, 0 }, { 20, 32, 2 } } }},
        {{ { { 0, 27, 4 }, { 32, 1, 98 } }, { { 0, 6, 0 }, { 32, 20, 2 } } }},
        {{ { { 27, 0, 4 }, { 1, 32, 98 } }, { { 6, 0, 0 }, { 20, 32, 2 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToDown25Sprites = {{
        {{ 21042, 21026, }},
        {{ 21043, 21027, }},
        {{ 21040, 21024, }},
        {{ 21041, 21025, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToDown25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown25ToDown60Sprites = {{
        {{ SPR_G2_FLUME_25_60_NW_SE_BACK + 0, SPR_G2_FLUME_25_60_NW_SE_BACK_WATER + 0, }},
        {{ SPR_G2_EMPTY + 0, SPR_G2_FLUME_25_60_NW_SE + 0, }},
        {{ SPR_G2_EMPTY + 0, SPR_G2_FLUME_25_60_NE_SW + 0, }},
        {{ SPR_G2_FLUME_25_60_NE_SW_BACK + 0, SPR_G2_FLUME_25_60_NE_SW_BACK_WATER + 0, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown25ToDown60BoundBoxes = {{
        {{ { { 0, 27, 4 }, { 32, 1, 42 } }, { { 0, 6, 0 }, { 32, 20, 0 } } }},
        {{ { { 27, 0, 4 }, { 1, 32, 42 } }, { { 6, 0, 0 }, { 20, 32, 0 } } }},
        {{ { { 0, 27, 4 }, { 32, 1, 42 } }, { { 0, 6, 0 }, { 32, 20, 0 } } }},
        {{ { { 27, 0, 4 }, { 1, 32, 42 } }, { { 6, 0, 0 }, { 20, 32, 0 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown25Sprites = {{
        {{ SPR_G2_FLUME_60_25_NW_SE_BACK + 0, SPR_G2_FLUME_60_25_NW_SE_BACK_WATER + 0, }},
        {{ SPR_G2_EMPTY + 0, SPR_G2_FLUME_60_25_NW_SE + 0, }},
        {{ SPR_G2_EMPTY + 0, SPR_G2_FLUME_60_25_NE_SW + 0, }},
        {{ SPR_G2_FLUME_60_25_NE_SW_BACK + 0, SPR_G2_FLUME_60_25_NE_SW_BACK_WATER + 0, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown25BoundBoxes = {{
        {{ { { 0, 27, 4 }, { 32, 1, 42 } }, { { 0, 6, 0 }, { 32, 20, 0 } } }},
        {{ { { 27, 0, 4 }, { 1, 32, 42 } }, { { 6, 0, 0 }, { 20, 32, 0 } } }},
        {{ { { 0, 27, 4 }, { 32, 1, 42 } }, { { 0, 6, 0 }, { 32, 20, 0 } } }},
        {{ { { 27, 0, 4 }, { 1, 32, 42 } }, { { 6, 0, 0 }, { 20, 32, 0 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown25ToFlatSprites = {{
        {{ 21038, 21018, }},
        {{ 21039, 21019, }},
        {{ 21036, 21016, }},
        {{ 21037, 21017, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown25ToFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            {{ 21116, 21100, }},
            {{ 21117, 21101, }},
            {{ 21118, 21102, }},
            {{ 21119, 21103, }},
        }},
        {{
            {{ 21088, 21104, }},
            {{ 21089, 21105, }},
            {{ 21090, 21106, }},
            {{ 21091, 21107, }},
        }},
        {{
            {{ 21087, 21103, }},
            {{ 21086, 21102, }},
            {{ 21085, 21101, }},
            {{ 21084, 21100, }},
        }},
        {{
            {{ 21123, 21107, }},
            {{ 21122, 21106, }},
            {{ 21121, 21105, }},
            {{ 21120, 21104, }},
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
            {{ 21124, 21108, }},
            {{ 21125, 21109, }},
            {{ 21126, 21110, }},
            {{ 21127, 21111, }},
        }},
        {{
            {{ 21096, 21112, }},
            {{ 21097, 21113, }},
            {{ 21098, 21114, }},
            {{ 21099, 21115, }},
        }},
        {{
            {{ 21095, 21111, }},
            {{ 21094, 21110, }},
            {{ 21093, 21109, }},
            {{ 21092, 21108, }},
        }},
        {{
            {{ 21131, 21115, }},
            {{ 21130, 21114, }},
            {{ 21129, 21113, }},
            {{ 21128, 21112, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            {{ 21077, 21065, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21076, 21064, }},
            {{ 21075, 21063, }},
        }},
        {{
            {{ 21080, 21068, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21079, 21067, }},
            {{ 21078, 21066, }},
        }},
        {{
            {{ 21083, 21071, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21082, 21070, }},
            {{ 21081, 21069, }},
        }},
        {{
            {{ 21074, 21062, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21073, 21061, }},
            {{ 21072, 21060, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesSprites = {{
        {{
            {{ 21048, 21060, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21049, 21061, }},
            {{ 21050, 21062, }},
        }},
        {{
            {{ 21051, 21063, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21052, 21064, }},
            {{ 21053, 21065, }},
        }},
        {{
            {{ 21054, 21066, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21055, 21067, }},
            {{ 21056, 21068, }},
        }},
        {{
            {{ 21057, 21069, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21058, 21070, }},
            {{ 21059, 21071, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn3TilesBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLogFlumeReverserSprites = {{
        {{ 21004, 21008, }},
        {{ 21005, 21009, }},
        {{ 21006, 21010, }},
        {{ 21007, 21011, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLogFlumeReverserBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // Flat
        { &kStationSprites[0][0][0], nullptr, &kStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], nullptr, &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], nullptr, &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0][0], nullptr, &kUp25BoundBoxes[0][0], false }, // Up25
        { nullptr, nullptr, nullptr, false }, // Up60
        { &kFlatToUp25Sprites[0][0], nullptr, &kFlatToUp25BoundBoxes[0][0], false }, // FlatToUp25
        { nullptr, nullptr, nullptr, false }, // Up25ToUp60
        { nullptr, nullptr, nullptr, false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], nullptr, &kUp25ToFlatBoundBoxes[0][0], false }, // Up25ToFlat
        { &kDown25Sprites[0][0], nullptr, &kDown25BoundBoxes[0][0], false }, // Down25
        { &kDown60Sprites[0][0], nullptr, &kDown60BoundBoxes[0][0], false }, // Down60
        { &kFlatToDown25Sprites[0][0], nullptr, &kFlatToDown25BoundBoxes[0][0], false }, // FlatToDown25
        { &kDown25ToDown60Sprites[0][0], nullptr, &kDown25ToDown60BoundBoxes[0][0], false }, // Down25ToDown60
        { &kDown60ToDown25Sprites[0][0], nullptr, &kDown60ToDown25BoundBoxes[0][0], false }, // Down60ToDown25
        { &kDown25ToFlatSprites[0][0], nullptr, &kDown25ToFlatBoundBoxes[0][0], false }, // Down25ToFlat
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn5Tiles
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
        { &kSBendLeftSprites[0][0][0], nullptr, &kSBendLeftBoundBoxes[0][0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0][0], nullptr, &kSBendRightBoundBoxes[0][0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { &kLeftQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn3Tiles
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
        { &kFlatSprites[0][0], nullptr, &kStationBoundBoxes[0][0], false }, // OnRidePhoto
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
        { &kLogFlumeReverserSprites[0][0], nullptr, &kLogFlumeReverserBoundBoxes[0][0], false }, // LogFlumeReverser
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
} // namespace OpenRCT2::LogFlume
