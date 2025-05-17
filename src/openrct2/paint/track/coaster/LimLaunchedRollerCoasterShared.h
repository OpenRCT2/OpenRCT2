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

namespace OpenRCT2::LimLaunchedRollerCoaster
{
    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kStationSprites = {{
        15018,
        15019,
        15018,
        15019,
    }};

    static constexpr std::array<std::array<ImageIndex, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            15388,
            15389,
            15390,
            15391,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15392,
            15393,
            15394,
            15395,
        }},
        {{
            15396,
            15397,
            15398,
            15399,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15400,
            15401,
            15402,
            15403,
        }},
        {{
            15395,
            15394,
            15393,
            15392,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15391,
            15390,
            15389,
            15388,
        }},
        {{
            15403,
            15402,
            15401,
            15400,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15399,
            15398,
            15397,
            15396,
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
            15419,
            15418,
            15417,
            15416,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15415,
            15414,
            15413,
            15412,
        }},
        {{
            15411,
            15410,
            15409,
            15408,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15407,
            15406,
            15405,
            15404,
        }},
        {{
            15412,
            15413,
            15414,
            15415,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15416,
            15417,
            15418,
            15419,
        }},
        {{
            15404,
            15405,
            15406,
            15407,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15408,
            15409,
            15410,
            15411,
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

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpSprites = {{
        {{
            {{ 15758, 15782, }},
            {{ 15759, 15783, }},
            {{ 15760, 15784, }},
        }},
        {{
            {{ 15761, 15785, }},
            {{ 15762, 15786, }},
            {{ 15763, 15787, }},
        }},
        {{
            {{ 15764, 15788, }},
            {{ 15765, 15789, }},
            {{ 15766, 15790, }},
        }},
        {{
            {{ 15767, 15791, }},
            {{ 15768, 15792, }},
            {{ 15769, 15793, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftTwistDownToUpBoundBoxes = {{
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
    kRightTwistDownToUpSprites = {{
        {{
            {{ 15770, 15794, }},
            {{ 15771, 15795, }},
            {{ 15772, 15796, }},
        }},
        {{
            {{ 15773, 15797, }},
            {{ 15774, 15798, }},
            {{ 15775, 15799, }},
        }},
        {{
            {{ 15776, 15800, }},
            {{ 15777, 15801, }},
            {{ 15778, 15802, }},
        }},
        {{
            {{ 15779, 15803, }},
            {{ 15780, 15804, }},
            {{ 15781, 15805, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightTwistDownToUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftTwistDownToUpBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kHalfLoopUpSprites = {{
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 0,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 1,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 2,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 3,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 4,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 5,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 6,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 7,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 8,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 9,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 10,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 11,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 12,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 13,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 14,
            SPR_G2_LIM_LAUNCHED_TRACK_HALF_LOOP + 15,
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
            15734,
            15735,
            15736,
        }},
        {{
            15737,
            15738,
            15739,
        }},
        {{
            15740,
            15741,
            15742,
        }},
        {{
            15743,
            15744,
            15745,
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
            15746,
            15747,
            15748,
        }},
        {{
            15749,
            15750,
            15751,
        }},
        {{
            15752,
            15753,
            15754,
        }},
        {{
            15755,
            15756,
            15757,
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 3>, kNumOrthogonalDirections>
    kRightCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftCorkscrewUpBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        15018,
        15019,
        15018,
        15019,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBoosterSprites = {{
        {{ 15012, 15014, }},
        {{ 15013, 15015, }},
        {{ 15012, 15014, }},
        {{ 15013, 15015, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBoosterBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp90Sprites = {{
        {{
            15702,
            kImageIndexUndefined,
        }},
        {{
            15703,
            kImageIndexUndefined,
        }},
        {{
            15704,
            kImageIndexUndefined,
        }},
        {{
            15705,
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
            15694,
            kImageIndexUndefined,
        }},
        {{
            15695,
            kImageIndexUndefined,
        }},
        {{
            15696,
            kImageIndexUndefined,
        }},
        {{
            15697,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp90BoundBoxes = {{
        {{
            { { 4, 6, 8 }, { 2, 20, 55 } },
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
            { { 6, 4, 8 }, { 20, 2, 55 } },
            {},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kDown90ToDown60Sprites = {{
        15696,
        15697,
        15694,
        15695,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kDown90ToDown60BoundBoxes = {{
        { { 24, 6, 0 }, { 2, 20, 55 } },
        { { 6, 4, 8 }, { 20, 2, 55 } },
        { { 4, 6, 8 }, { 2, 20, 55 } },
        { { 6, 24, 0 }, { 20, 2, 55 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp90ToUp60Sprites = {{
        15698,
        15699,
        15700,
        15701,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp90ToUp60BoundBoxes = {{
        { { 4, 6, 8 }, { 2, 20, 48 } },
        { { 6, 39, 8 }, { 20, 2, 31 } },
        { { 39, 6, 8 }, { 2, 20, 31 } },
        { { 6, 4, 8 }, { 20, 2, 48 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kDown60ToDown90Sprites = {{
        {{
            15700,
            kImageIndexUndefined,
        }},
        {{
            15701,
            kImageIndexUndefined,
        }},
        {{
            15698,
            kImageIndexUndefined,
        }},
        {{
            15699,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kDown60ToDown90BoundBoxes = {{
        {{
            { { 39, 6, 8 }, { 2, 20, 31 } },
            {},
        }},
        {{
            { { 6, 0, 2 }, { 20, 6, 3 } },
            {},
        }},
        {{
            { { 0, 6, 2 }, { 6, 20, 3 } },
            {},
        }},
        {{
            { { 6, 39, 8 }, { 20, 2, 31 } },
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftBarrelRollUpToDownSprites = {{
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 0, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 1, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 2, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 3, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 4, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 5, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 6, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 7, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 8, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 9, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 10, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 12, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 13, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 14, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 15, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 16, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 17, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 18, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 19, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 20, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 21, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 22, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 23, }},
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
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 24, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 25, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 26, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 27, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 28, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 29, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 30, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 31, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 32, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 33, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 34, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 35, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 36, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 37, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 38, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 39, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 40, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 41, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 42, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 43, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 44, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 45, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 46, SPR_G2_LIM_LAUNCHED_TRACK_BARREL_ROLL + 47, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightBarrelRollUpToDownBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBarrelRollUpToDownBoundBoxes);

    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kLeftLargeHalfLoopUpSprites = {{
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 0,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 1,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 2,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 3,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 4,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 5,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 6,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 7,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 8,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 9,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 10,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 11,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 12,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 13,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 14,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 15,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 16,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 17,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 18,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 19,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 20,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 21,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 22,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 23,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 24,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 25,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 26,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 27,
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
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 28,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 29,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 30,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 31,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 32,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 33,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 34,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 35,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 36,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 37,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 38,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 39,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 40,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 41,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 42,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 43,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 44,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 45,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 46,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 47,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 48,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 49,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 50,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 51,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 52,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 53,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 54,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_HALF_LOOP + 55,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightLargeHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 3>, kNumOrthogonalDirections>
    kUp90ToInvertedFlatQuarterLoopSprites = {{
        {{
            15722,
            15723,
            15724,
        }},
        {{
            15725,
            15726,
            15727,
        }},
        {{
            15728,
            15729,
            15730,
        }},
        {{
            15731,
            15732,
            15733,
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
            15724,
            15723,
            15722,
            kImageIndexUndefined,
        }},
        {{
            15727,
            15726,
            15725,
            kImageIndexUndefined,
        }},
        {{
            15730,
            15729,
            15728,
            kImageIndexUndefined,
        }},
        {{
            15733,
            15732,
            15731,
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
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            15020,
            15021,
            15020,
            15021,
        }},
        {{
            15022,
            15023,
            15022,
            15023,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90Sprites = {{
        {{
            {{ 15706, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15707, 15715, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15716, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15709, 15717, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp90BoundBoxes = {{
        {{
            {{ { { 4, 6, 8 }, { 2, 20, 63 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 63 } }, { { 6, 24, 8 }, { 20, 2, 63 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 24, 24, 8 }, { 2, 2, 63 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { 6, 4, 8 }, { 20, 2, 63 } }, { { 6, 24, 8 }, { 20, 2, 63 } } }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90Sprites = {{
        {{
            {{ 15710, 15718, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15719, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15712, 15720, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15713, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp90BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp90BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 6>, kNumOrthogonalDirections>
    kLeftLargeCorkscrewUpSprites = {{
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 0,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 1,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 2,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 3,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 4,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 5,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 6,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 7,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 8,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 9,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 10,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 11,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 12,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 13,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 14,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 15,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 16,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 17,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 18,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 19,
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
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 20,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 21,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 22,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 23,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 24,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 25,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 26,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 27,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 28,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 29,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 30,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 31,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 32,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 33,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 34,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 35,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 36,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 37,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 38,
            SPR_G2_LIM_LAUNCHED_TRACK_LARGE_CORKSCREW + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 6>, kNumOrthogonalDirections>
    kRightLargeCorkscrewUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeCorkscrewUpBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftMediumHalfLoopUpSprites = {{
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 0,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 1,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 2,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 3,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 4,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 5,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 6,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 7,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 8,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 9,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 10,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 11,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 12,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 13,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 14,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 15,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 16,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 17,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 18,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 19,
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
            { { 0, 0, 140 }, { 32, 32, 3 } },
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
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 20,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 21,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 22,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 23,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 24,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 25,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 26,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 27,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 28,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 29,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 30,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 31,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 32,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 33,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 34,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 35,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 36,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 37,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 38,
            SPR_G2_LIM_LAUNCHED_TRACK_MEDIUM_HALF_LOOP + 39,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightMediumHalfLoopUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftMediumHalfLoopUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 0, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 1, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 2, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 4, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 5, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 9, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 10, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 11, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 13, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 14, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 15, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kLeftZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 28 }, { 32, 20, 1 } } }},
            {{ { { 0, 6, 28 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 28 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 31, 0, 0 }, { 1, 32, 32 } } }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 28 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 6, 33 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 6, 0, 33 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 17, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 18, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 19, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 22, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 23, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 24, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 25, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 26, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 28, SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 29, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 30, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 3>, kNumOrthogonalDirections>
    kRightZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftZeroGRollUpBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 0, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 1, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 2, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 3, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 5, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 6, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 7, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 8, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 9, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 10, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 11, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 12, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 13, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 14, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 15, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 16, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 17, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 18, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 19, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftLargeZeroGRollUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 30, 0 }, { 32, 0, 96 } } }},
            {{ { { 0, 30, 0 }, { 32, 0, 64 } }, {} }},
            {{ { { 0, 26, 0 }, { 26, 0, 20 } }, {} }},
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
            {{ { { 18, 0, 0 }, { 10, 32, 20 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpSprites = {{
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 20, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 21, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 22, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 23, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 24, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 25, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 26, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 27, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 28, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 29, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 30, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 31, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 32, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 33, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 34, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 35, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 36, SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 37, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 38, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_LARGE_ZERO_G_ROLL + 39, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightLargeZeroGRollUpBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftLargeZeroGRollUpBoundBoxes);
    // clang-format on
} // namespace OpenRCT2::LimLaunchedRollerCoaster
