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

namespace OpenRCT2::JuniorRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            27807,
            27808,
            27807,
            27808,
        }},
        {{
            27913,
            27914,
            27913,
            27914,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kStationSprites = {{
        28189,
        28190,
        28189,
        28190,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections> kStationBoundBoxes = { {
        { { 0, 6, 1 }, { 32, 20, 1 } },
        { { 6, 0, 1 }, { 20, 32, 1 } },
        { { 0, 6, 1 }, { 32, 20, 1 } },
        { { 6, 0, 1 }, { 20, 32, 1 } },
    } };
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            27809,
            27821,
            27813,
            27823,
        }},
        {{
            27915,
            27927,
            27919,
            27929,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp60Sprites = {{
        {{
            27810,
            27822,
            27814,
            27824,
        }},
        {{
            27916,
            27928,
            27920,
            27930,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp60Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 10, 29, 0 }, { 10, 1, 75 } },
        { { 29, 10, 0 }, { 1, 10, 75 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            27811,
            27825,
            27815,
            27827,
        }},
        {{
            27917,
            27931,
            27921,
            27933,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToUp60Sprites = {{
        {{
            {{ 27812, kImageIndexUndefined, }},
            {{ 27855, 27826, }},
            {{ 27853, 27816, }},
            {{ 27828, kImageIndexUndefined, }},
        }},
        {{
            {{ 27918, kImageIndexUndefined, }},
            {{ 27941, 27932, }},
            {{ 27939, 27922, }},
            {{ 27934, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Offsets = {{
        {{ { 0, 6, 0 }, {} }},
        {{ { 6, 0, 0 }, { 6, 0, 0 } }},
        {{ { 0, 6, 0 }, { 0, 6, 0 } }},
        {{ { 6, 0, 0 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        {{ { { 10, 0, 0 }, { 10, 32, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 0, 10, 0 }, { 32, 10, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToUp25Sprites = {{
        {{
            {{ 27820, kImageIndexUndefined, }},
            {{ 27856, 27832, }},
            {{ 27854, 27818, }},
            {{ 27830, kImageIndexUndefined, }},
        }},
        {{
            {{ 27926, kImageIndexUndefined, }},
            {{ 27942, 27938, }},
            {{ 27940, 27924, }},
            {{ 27936, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Offsets = {{
        {{ { 0, 6, 0 }, {} }},
        {{ { 6, 0, 0 }, { 6, 0, 0 } }},
        {{ { 0, 6, 0 }, { 0, 6, 0 } }},
        {{ { 6, 0, 0 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        {{ { { 10, 0, 0 }, { 10, 32, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 0, 10, 0 }, { 32, 10, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            27819,
            27831,
            27817,
            27829,
        }},
        {{
            27925,
            27937,
            27923,
            27935,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToFlatOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesSprites = {{
        {{
            27833,
            kImageIndexUndefined,
            27834,
            27835,
            kImageIndexUndefined,
            27836,
            27837,
        }},
        {{
            27838,
            kImageIndexUndefined,
            27839,
            27840,
            kImageIndexUndefined,
            27841,
            27842,
        }},
        {{
            27843,
            kImageIndexUndefined,
            27844,
            27845,
            kImageIndexUndefined,
            27846,
            27847,
        }},
        {{
            27848,
            kImageIndexUndefined,
            27849,
            27850,
            kImageIndexUndefined,
            27851,
            27852,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesOffsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 16, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        {{ 27997, 28005, }},
        {{ 27998, 28006, }},
        {{ 27999, kImageIndexUndefined, }},
        {{ 28000, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        {{ 28001, kImageIndexUndefined, }},
        {{ 28002, kImageIndexUndefined, }},
        {{ 28003, 28007, }},
        {{ 28004, 28008, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedRightQuarterTurn5TilesSprites = {{
        {{
            {{ 28037, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28038, kImageIndexUndefined, }},
            {{ 28039, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28040, kImageIndexUndefined, }},
            {{ 28041, kImageIndexUndefined, }},
        }},
        {{
            {{ 28042, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28043, kImageIndexUndefined, }},
            {{ 28044, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28045, kImageIndexUndefined, }},
            {{ 28046, 28057, }},
        }},
        {{
            {{ 28047, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28048, kImageIndexUndefined, }},
            {{ 28049, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28050, kImageIndexUndefined, }},
            {{ 28051, kImageIndexUndefined, }},
        }},
        {{
            {{ 28052, 28058, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28053, kImageIndexUndefined, }},
            {{ 28054, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28055, kImageIndexUndefined, }},
            {{ 28056, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kBankedRightQuarterTurn5TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        }},
        {{
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        {{ 28009, 28013, }},
        {{ 28010, 28014, }},
        {{ 28011, kImageIndexUndefined, }},
        {{ 28012, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        {{ 28015, kImageIndexUndefined, }},
        {{ 28016, kImageIndexUndefined, }},
        {{ 28017, 28019, }},
        {{ 28018, 28020, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 28021, 28025, }},
        {{ 28022, 28026, }},
        {{ 28023, kImageIndexUndefined, }},
        {{ 28024, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        {{ 28027, kImageIndexUndefined, }},
        {{ 28028, kImageIndexUndefined, }},
        {{ 28029, 28031, }},
        {{ 28030, 28032, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        28033,
        28034,
        28035,
        28036,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        { { 0, 27, 0 }, { 32, 1, 26 } },
        { { 27, 0, 0 }, { 1, 32, 26 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>, 2>
    kLeftQuarterTurn5TilesUp25Sprites = {{
        {{
            {{
                27877,
                kImageIndexUndefined,
                27878,
                27879,
                kImageIndexUndefined,
                27880,
                27881,
            }},
            {{
                27882,
                kImageIndexUndefined,
                27883,
                27884,
                kImageIndexUndefined,
                27885,
                27886,
            }},
            {{
                27887,
                kImageIndexUndefined,
                27888,
                27889,
                kImageIndexUndefined,
                27890,
                27891,
            }},
            {{
                27892,
                kImageIndexUndefined,
                27893,
                27894,
                kImageIndexUndefined,
                27895,
                27896,
            }},
        }},
        {{
            {{
                27963,
                kImageIndexUndefined,
                27964,
                27965,
                kImageIndexUndefined,
                27966,
                27967,
            }},
            {{
                27968,
                kImageIndexUndefined,
                27969,
                27970,
                kImageIndexUndefined,
                27971,
                27972,
            }},
            {{
                27973,
                kImageIndexUndefined,
                27974,
                27975,
                kImageIndexUndefined,
                27976,
                27977,
            }},
            {{
                27978,
                kImageIndexUndefined,
                27979,
                27980,
                kImageIndexUndefined,
                27981,
                27982,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 16, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>, 2>
    kRightQuarterTurn5TilesUp25Sprites = {{
        {{
            {{
                27857,
                kImageIndexUndefined,
                27858,
                27859,
                kImageIndexUndefined,
                27860,
                27861,
            }},
            {{
                27862,
                kImageIndexUndefined,
                27863,
                27864,
                kImageIndexUndefined,
                27865,
                27866,
            }},
            {{
                27867,
                kImageIndexUndefined,
                27868,
                27869,
                kImageIndexUndefined,
                27870,
                27871,
            }},
            {{
                27872,
                kImageIndexUndefined,
                27873,
                27874,
                kImageIndexUndefined,
                27875,
                27876,
            }},
        }},
        {{
            {{
                27943,
                kImageIndexUndefined,
                27944,
                27945,
                kImageIndexUndefined,
                27946,
                27947,
            }},
            {{
                27948,
                kImageIndexUndefined,
                27949,
                27950,
                kImageIndexUndefined,
                27951,
                27952,
            }},
            {{
                27953,
                kImageIndexUndefined,
                27954,
                27955,
                kImageIndexUndefined,
                27956,
                27957,
            }},
            {{
                27958,
                kImageIndexUndefined,
                27959,
                27960,
                kImageIndexUndefined,
                27961,
                27962,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25Offsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 16, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 16, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            27901,
            27902,
            27903,
            27904,
        }},
        {{
            27908,
            27907,
            27906,
            27905,
        }},
        {{
            27904,
            27903,
            27902,
            27901,
        }},
        {{
            27905,
            27906,
            27907,
            27908,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendLeftBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 0, 0 }, { 32, 26, 1 } },
            { { 0, 6, 0 }, { 32, 26, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 0, 0, 0 }, { 26, 32, 1 } },
            { { 6, 0, 0 }, { 26, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 6, 0 }, { 32, 26, 1 } },
            { { 0, 0, 0 }, { 32, 26, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 6, 0, 0 }, { 26, 32, 1 } },
            { { 0, 0, 0 }, { 26, 32, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendRightSprites = {{
        {{
            27897,
            27898,
            27899,
            27900,
        }},
        {{
            27912,
            27911,
            27910,
            27909,
        }},
        {{
            27900,
            27899,
            27898,
            27897,
        }},
        {{
            27909,
            27910,
            27911,
            27912,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightOffsets = {{
        {{
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            { 0, 0, 0 }, 
            { 0, 6, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesSprites = {{
        {{
            28059,
            kImageIndexUndefined,
            28060,
            28061,
        }},
        {{
            28062,
            kImageIndexUndefined,
            28063,
            28064,
        }},
        {{
            28065,
            kImageIndexUndefined,
            28066,
            28067,
        }},
        {{
            28068,
            kImageIndexUndefined,
            28069,
            28070,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesOffsets = {{
        {{
            { 0, 6, 0 }, 
            {},
            { 16, 16, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 16, 0, 0 }, 
            { 0, 6, 0 }, 
        }},
        {{
            { 0, 6, 0 }, 
            {},
            { 0, 0, 0 }, 
            { 6, 0, 0 }, 
        }},
        {{
            { 6, 0, 0 }, 
            {},
            { 0, 16, 0 }, 
            { 0, 6, 0 }, 
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 16, 0, 0 }, { 16, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 1 } },
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TilesSprites = {{
        {{
            {{ 28071, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28072, kImageIndexUndefined, }},
            {{ 28073, kImageIndexUndefined, }},
        }},
        {{
            {{ 28074, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28075, kImageIndexUndefined, }},
            {{ 28076, 28083, }},
        }},
        {{
            {{ 28077, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28078, kImageIndexUndefined, }},
            {{ 28079, kImageIndexUndefined, }},
        }},
        {{
            {{ 28080, 28084, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28081, kImageIndexUndefined, }},
            {{ 28082, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        }},
        {{
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesUp25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{
                28085,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28086,
            }},
            {{
                28087,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28088,
            }},
            {{
                28089,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28090,
            }},
            {{
                28091,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28092,
            }},
        }},
        {{
            {{
                28101,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28102,
            }},
            {{
                28103,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28104,
            }},
            {{
                28105,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28106,
            }},
            {{
                28107,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28108,
            }},
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

    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kRightQuarterTurn3TilesDown25Sprites = {{
        {{
            {{
                28093,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28094,
            }},
            {{
                28095,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28096,
            }},
            {{
                28097,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28098,
            }},
            {{
                28099,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28100,
            }},
        }},
        {{
            {{
                28109,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28110,
            }},
            {{
                28111,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28112,
            }},
            {{
                28113,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28114,
            }},
            {{
                28115,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28116,
            }},
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightQuarterTurn3TilesDown25Offsets = {{
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
    kRightQuarterTurn3TilesDown25BoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            {},
            {},
            { { 6, 0, 0 }, { 20, 32, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            {},
            {},
            { { 0, 6, 0 }, { 32, 20, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatToUp60Sprites = {{
        {{
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_SW_NE + 0, kImageIndexUndefined, }},
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_NW_SE + 0, SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_NW_SE_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_NE_SW + 0, SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_NE_SW_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_SE_NW + 0, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_CHAIN_SW_NE + 0, kImageIndexUndefined, }},
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_CHAIN_NW_SE + 0, SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_CHAIN_NW_SE_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_CHAIN_NE_SW + 0, SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_CHAIN_NE_SW_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_FLAT_TO_60_DEG_UP_CHAIN_SE_NW + 0, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp60Offsets = {{
        {{ { 12, -10, 24 }, {} }},
        {{ { 22, 9, 24 }, { 12, -11, 0 } }},
        {{ { 30, 7, 24 }, { 5, 0, 0 } }},
        {{ { 12, -12, 24 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp60BoundBoxes = {{
        {{ { { 0, 4, 0 }, { 32, 24, 1 } }, {} }},
        {{ { { 10, 0, 0 }, { 10, 1, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 0, 10, 0 }, { 1, 10, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 4, 0, 0 }, { 24, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToFlatSprites = {{
        {{
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_SW_NE + 0, kImageIndexUndefined, }},
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_NW_SE + 0, SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_NW_SE_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_NE_SW + 0, SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_NE_SW_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_SE_NW + 0, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_CHAIN_SW_NE + 0, kImageIndexUndefined, }},
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_CHAIN_NW_SE + 0, SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_CHAIN_NW_SE_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_CHAIN_NE_SW + 0, SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_CHAIN_NE_SW_PART_0_2 + 0, }},
            {{ SPR_JUNIOR_RC_60_DEG_UP_TO_FLAT_CHAIN_SE_NW + 0, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToFlatOffsets = {{
        {{ { 13, -9, 24 }, {} }},
        {{ { 32, 9, 24 }, { -7, -11, 0 } }},
        {{ { 22, 18, 24 }, { 2, -19, 0 } }},
        {{ { 13, -9, 24 }, {} }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToFlatBoundBoxes = {{
        {{ { { 0, 4, 0 }, { 32, 24, 1 } }, {} }},
        {{ { { 10, 0, 0 }, { 10, 1, 43 } }, { { 4, 0, 0 }, { 2, 32, 43 } } }},
        {{ { { 0, 10, 0 }, { 1, 10, 43 } }, { { 0, 4, 0 }, { 32, 2, 43 } } }},
        {{ { { 4, 0, 0 }, { 24, 32, 1 } }, {} }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 28117, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28118, kImageIndexUndefined, }},
            {{ 28119, kImageIndexUndefined, }},
            {{ 28120, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28121, kImageIndexUndefined, }},
            {{ 28122, 28129, }},
        }},
        {{
            {{ 28120, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28121, kImageIndexUndefined, }},
            {{ 28122, 28129, }},
            {{ 28123, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28124, kImageIndexUndefined, }},
            {{ 28125, kImageIndexUndefined, }},
        }},
        {{
            {{ 28123, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28124, kImageIndexUndefined, }},
            {{ 28125, kImageIndexUndefined, }},
            {{ 28126, 28130, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28127, kImageIndexUndefined, }},
            {{ 28128, kImageIndexUndefined, }},
        }},
        {{
            {{ 28126, 28130, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28127, kImageIndexUndefined, }},
            {{ 28128, kImageIndexUndefined, }},
            {{ 28117, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28118, kImageIndexUndefined, }},
            {{ 28119, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixDownSmallSprites = {{
        {{
            {{ 28131, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28132, kImageIndexUndefined, }},
            {{ 28133, kImageIndexUndefined, }},
            {{ 28134, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28135, kImageIndexUndefined, }},
            {{ 28136, 28143, }},
        }},
        {{
            {{ 28134, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28135, kImageIndexUndefined, }},
            {{ 28136, 28143, }},
            {{ 28137, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28138, kImageIndexUndefined, }},
            {{ 28139, kImageIndexUndefined, }},
        }},
        {{
            {{ 28137, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28138, kImageIndexUndefined, }},
            {{ 28139, kImageIndexUndefined, }},
            {{ 28140, 28144, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28141, kImageIndexUndefined, }},
            {{ 28142, kImageIndexUndefined, }},
        }},
        {{
            {{ 28140, 28144, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28141, kImageIndexUndefined, }},
            {{ 28142, kImageIndexUndefined, }},
            {{ 28131, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28132, kImageIndexUndefined, }},
            {{ 28133, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixDownSmallBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
        }},
        {{
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 16, 1 } }, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 28145, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28146, kImageIndexUndefined, }},
            {{ 28147, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28148, kImageIndexUndefined, }},
            {{ 28149, kImageIndexUndefined, }},
            {{ 28150, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28151, kImageIndexUndefined, }},
            {{ 28152, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28153, kImageIndexUndefined, }},
            {{ 28154, 28165, }},
        }},
        {{
            {{ 28150, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28151, kImageIndexUndefined, }},
            {{ 28152, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28153, kImageIndexUndefined, }},
            {{ 28154, 28165, }},
            {{ 28155, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28156, kImageIndexUndefined, }},
            {{ 28157, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28158, kImageIndexUndefined, }},
            {{ 28159, kImageIndexUndefined, }},
        }},
        {{
            {{ 28155, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28156, kImageIndexUndefined, }},
            {{ 28157, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28158, kImageIndexUndefined, }},
            {{ 28159, kImageIndexUndefined, }},
            {{ 28160, 28166, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28161, kImageIndexUndefined, }},
            {{ 28162, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28163, kImageIndexUndefined, }},
            {{ 28164, kImageIndexUndefined, }},
        }},
        {{
            {{ 28160, 28166, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28161, kImageIndexUndefined, }},
            {{ 28162, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28163, kImageIndexUndefined, }},
            {{ 28164, kImageIndexUndefined, }},
            {{ 28145, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28146, kImageIndexUndefined, }},
            {{ 28147, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28148, kImageIndexUndefined, }},
            {{ 28149, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixDownLargeSprites = {{
        {{
            {{ 28167, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28168, kImageIndexUndefined, }},
            {{ 28169, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28170, kImageIndexUndefined, }},
            {{ 28171, kImageIndexUndefined, }},
            {{ 28172, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28173, kImageIndexUndefined, }},
            {{ 28174, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28175, kImageIndexUndefined, }},
            {{ 28176, 28187, }},
        }},
        {{
            {{ 28172, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28173, kImageIndexUndefined, }},
            {{ 28174, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28175, kImageIndexUndefined, }},
            {{ 28176, 28187, }},
            {{ 28177, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28178, kImageIndexUndefined, }},
            {{ 28179, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28180, kImageIndexUndefined, }},
            {{ 28181, kImageIndexUndefined, }},
        }},
        {{
            {{ 28177, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28178, kImageIndexUndefined, }},
            {{ 28179, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28180, kImageIndexUndefined, }},
            {{ 28181, kImageIndexUndefined, }},
            {{ 28182, 28188, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28183, kImageIndexUndefined, }},
            {{ 28184, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28185, kImageIndexUndefined, }},
            {{ 28186, kImageIndexUndefined, }},
        }},
        {{
            {{ 28182, 28188, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28183, kImageIndexUndefined, }},
            {{ 28184, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28185, kImageIndexUndefined, }},
            {{ 28186, kImageIndexUndefined, }},
            {{ 28167, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28168, kImageIndexUndefined, }},
            {{ 28169, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28170, kImageIndexUndefined, }},
            {{ 28171, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixDownLargeBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
        }},
        {{
            {{ { { 0, 27, 0 }, { 32, 1, 26 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 32, 16, 1 } }, {} }},
            {{ { { 16, 16, 27 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 27 }, { 16, 32, 1 } }, {} }},
            {{ { { 27, 0, 0 }, { 1, 32, 26 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 1 } }, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 1 } }, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 1 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 1 } }, {} }},
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBrakesSprites = {{
        28191,
        28192,
        28191,
        28192,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBrakesOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBoosterSprites = {{
        SPR_JUNIOR_RC_BOOSTER_NW_SE + 0,
        SPR_JUNIOR_RC_BOOSTER_NE_SW + 0,
        SPR_JUNIOR_RC_BOOSTER_NW_SE + 0,
        SPR_JUNIOR_RC_BOOSTER_NE_SW + 0,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBoosterBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            28301,
            28302,
            28303,
            kImageIndexUndefined,
            28304,
        }},
        {{
            28305,
            28306,
            28307,
            kImageIndexUndefined,
            28308,
        }},
        {{
            28309,
            28310,
            28311,
            kImageIndexUndefined,
            28312,
        }},
        {{
            28313,
            28314,
            28315,
            kImageIndexUndefined,
            28316,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 0, 0, 0 }, { 16, 34, 1 } },
            { { 16, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 0, 0 }, { 18, 16, 1 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            28285,
            28286,
            28287,
            kImageIndexUndefined,
            28288,
        }},
        {{
            28289,
            28290,
            28291,
            kImageIndexUndefined,
            28292,
        }},
        {{
            28293,
            28294,
            28295,
            kImageIndexUndefined,
            28296,
        }},
        {{
            28297,
            28298,
            28299,
            kImageIndexUndefined,
            28300,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            28333,
            28334,
            28335,
            kImageIndexUndefined,
            28336,
        }},
        {{
            28337,
            28338,
            28339,
            kImageIndexUndefined,
            28340,
        }},
        {{
            28341,
            28342,
            28343,
            kImageIndexUndefined,
            28344,
        }},
        {{
            28345,
            28346,
            28347,
            kImageIndexUndefined,
            28348,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagBoundBoxes = {{
        {{
            { { 0, 27, 0 }, { 32, 1, 26 } },
            { { 0, 0, 0 }, { 32, 16, 1 } },
            { { 0, 16, 0 }, { 16, 16, 1 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 1 } },
        }},
        {{
            { { 27, 0, 0 }, { 1, 32, 26 } },
            { { 0, 0, 27 }, { 16, 34, 0 } },
            { { 16, 16, 27 }, { 16, 16, 0 } },
            {},
            { { 16, 0, 27 }, { 18, 16, 0 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 1 } },
            { { 0, 16, 0 }, { 32, 16, 1 } },
            { { 16, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 1 } },
            { { 16, 0, 0 }, { 16, 32, 1 } },
            { { 0, 0, 0 }, { 16, 16, 1 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 1 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            28317,
            28318,
            28319,
            kImageIndexUndefined,
            28320,
        }},
        {{
            28321,
            28322,
            28323,
            kImageIndexUndefined,
            28324,
        }},
        {{
            28325,
            28326,
            28327,
            kImageIndexUndefined,
            28328,
        }},
        {{
            28329,
            28330,
            28331,
            kImageIndexUndefined,
            28332,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                28197,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28198,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28199,
                kImageIndexUndefined,
            }},
            {{
                28200,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28233,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28234,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28235,
                kImageIndexUndefined,
            }},
            {{
                28236,
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
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
                28209,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28210,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28211,
                kImageIndexUndefined,
            }},
            {{
                28212,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28245,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28246,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28247,
                kImageIndexUndefined,
            }},
            {{
                28248,
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                28221,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28222,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28223,
                kImageIndexUndefined,
            }},
            {{
                28224,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28257,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28258,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28259,
                kImageIndexUndefined,
            }},
            {{
                28260,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
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
                28201,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28202,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28203,
                kImageIndexUndefined,
            }},
            {{
                28204,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28237,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28238,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28239,
                kImageIndexUndefined,
            }},
            {{
                28240,
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25ToUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                28213,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28214,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28215,
                kImageIndexUndefined,
            }},
            {{
                28216,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28249,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28250,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28251,
                kImageIndexUndefined,
            }},
            {{
                28252,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp60ToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                28217,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28218,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28219,
                kImageIndexUndefined,
            }},
            {{
                28220,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28253,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28254,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28255,
                kImageIndexUndefined,
            }},
            {{
                28256,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { 0, 0, 0 }, { 16, 16, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
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
                28205,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28206,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28207,
                kImageIndexUndefined,
            }},
            {{
                28208,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28241,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28242,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28243,
                kImageIndexUndefined,
            }},
            {{
                28244,
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatToUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                28225,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28226,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28227,
                kImageIndexUndefined,
            }},
            {{
                28228,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28261,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28262,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28263,
                kImageIndexUndefined,
            }},
            {{
                28264,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp60ToFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                28229,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28230,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28231,
                kImageIndexUndefined,
            }},
            {{
                28232,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                28265,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                28266,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                28267,
                kImageIndexUndefined,
            }},
            {{
                28268,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28353, 28357, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28354, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28355, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 28356, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28358, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28359, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28360, 28362, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 28361, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28373, 28377, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28374, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28375, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 28376, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28378, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28379, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28380, 28382, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 28381, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28363, 28367, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28364, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28365, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 28366, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28368, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28369, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 28370, 28372, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 28371, kImageIndexUndefined, }},
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
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 1 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            28349,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            28350,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            28351,
            kImageIndexUndefined,
        }},
        {{
            28352,
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
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            28193,
            28194,
            28193,
            28194,
        }},
        {{
            28195,
            28196,
            28195,
            28196,
        }},
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kBlockBrakesOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
        { { 0, 6, 0 }, { 32, 20, 1 } },
        { { 6, 0, 0 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            kImageIndexUndefined,
            SPR_G2_JUNIOR_DIAG_BRAKES + 0,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_JUNIOR_DIAG_BRAKES + 1,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_JUNIOR_DIAG_BRAKES + 0,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_JUNIOR_DIAG_BRAKES + 1,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBrakesOffsets = {{
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
    kDiagBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagBlockBrakesSprites = {{
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_JUNIOR_DIAG_BRAKES + 3,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_JUNIOR_DIAG_BRAKES + 5,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_JUNIOR_DIAG_BRAKES + 3,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_JUNIOR_DIAG_BRAKES + 5,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                SPR_G2_JUNIOR_DIAG_BRAKES + 2,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_JUNIOR_DIAG_BRAKES + 4,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                SPR_G2_JUNIOR_DIAG_BRAKES + 2,
                kImageIndexUndefined,
            }},
            {{
                SPR_G2_JUNIOR_DIAG_BRAKES + 4,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesOffsets = {{
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
    kDiagBlockBrakesBoundBoxes = {{
        {{
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
        }},
        {{
            {},
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
        }},
        {{
            {},
            {},
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
        }},
        {{
            { { -16, -16, 0 }, { 32, 32, 1 } },
            {},
            {},
            {},
        }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kFlatBoundBoxes[0], false }, // Flat
        { &kBlockBrakesSprites[0][0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // EndStation
        { &kStationSprites[0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // BeginStation
        { &kStationSprites[0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // MiddleStation
        { &kUp25Sprites[0][0], &kUp25Offsets[0], &kUp25BoundBoxes[0], false }, // Up25
        { &kUp60Sprites[0][0], &kUp60Offsets[0], &kUp60BoundBoxes[0], false }, // Up60
        { &kFlatToUp25Sprites[0][0], &kFlatToUp25Offsets[0], &kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0][0], &kUp25ToUp60Offsets[0][0], &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0][0], &kUp60ToUp25Offsets[0][0], &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0], &kUp25ToFlatOffsets[0], &kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5Tiles
        { &kRightQuarterTurn5TilesSprites[0][0], &kRightQuarterTurn5TilesOffsets[0][0], &kRightQuarterTurn5TilesBoundBoxes[0][0], false }, // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0][0], nullptr, &kFlatToLeftBankBoundBoxes[0][0], false }, // FlatToLeftBank
        { &kFlatToRightBankSprites[0][0], nullptr, &kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { nullptr, nullptr, nullptr, true }, // BankedLeftQuarterTurn5Tiles
        { &kBankedRightQuarterTurn5TilesSprites[0][0][0], nullptr, &kBankedRightQuarterTurn5TilesBoundBoxes[0][0][0], false }, // BankedRightQuarterTurn5Tiles
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
        { &kLeftQuarterTurn5TilesUp25Sprites[0][0][0], &kLeftQuarterTurn5TilesUp25Offsets[0][0], &kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &kRightQuarterTurn5TilesUp25Sprites[0][0][0], &kRightQuarterTurn5TilesUp25Offsets[0][0], &kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &kSBendLeftSprites[0][0], &kSBendLeftOffsets[0][0], &kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &kSBendRightSprites[0][0], &kSBendRightOffsets[0][0], &kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3Tiles
        { &kRightQuarterTurn3TilesSprites[0][0], &kRightQuarterTurn3TilesOffsets[0][0], &kRightQuarterTurn3TilesBoundBoxes[0][0], false }, // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3Tiles
        { &kRightBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kRightBankedQuarterTurn3TilesBoundBoxes[0][0][0], false }, // RightBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesUp25
        { &kRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightQuarterTurn3TilesUp25Offsets[0][0], &kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { &kRightQuarterTurn3TilesDown25Sprites[0][0][0], &kRightQuarterTurn3TilesDown25Offsets[0][0], &kRightQuarterTurn3TilesDown25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesDown25
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
        { &kFlatToUp60Sprites[0][0][0], &kFlatToUp60Offsets[0][0], &kFlatToUp60BoundBoxes[0][0], false }, // FlatToUp60
        { &kUp60ToFlatSprites[0][0][0], &kUp60ToFlatOffsets[0][0], &kUp60ToFlatBoundBoxes[0][0], false }, // Up60ToFlat
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
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { &kRightHalfBankedHelixDownSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixDownSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { &kRightHalfBankedHelixDownLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixDownLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0], &kBrakesOffsets[0], &kBrakesBoundBoxes[0], false }, // Brakes
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
        { nullptr, nullptr, nullptr, false }, // Up25LeftBanked
        { nullptr, nullptr, nullptr, false }, // Up25RightBanked
        { nullptr, nullptr, nullptr, false }, // Waterfall
        { nullptr, nullptr, nullptr, false }, // Rapids
        { &kFlatSprites[0][0], &kFlatOffsets[0], &kStationBoundBoxes[0], false }, // OnRidePhoto
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
        { &kDiagUp60Sprites[0][0][0], &kDiagUp60Offsets[0][0], &kDiagUp60BoundBoxes[0][0], false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0], &kDiagFlatToUp25Offsets[0][0], &kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0][0], &kDiagUp25ToUp60Offsets[0][0], &kDiagUp25ToUp60BoundBoxes[0][0], false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0][0], &kDiagUp60ToUp25Offsets[0][0], &kDiagUp60ToUp25BoundBoxes[0][0], false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0], &kDiagUp25ToFlatOffsets[0][0], &kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown60
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToFlat
        { &kDiagFlatToUp60Sprites[0][0][0], &kDiagFlatToUp60Offsets[0][0], &kDiagFlatToUp60BoundBoxes[0][0], false }, // DiagFlatToUp60
        { &kDiagUp60ToFlatSprites[0][0][0], &kDiagUp60ToFlatOffsets[0][0], &kDiagUp60ToFlatBoundBoxes[0][0], false }, // DiagUp60ToFlat
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
        { &kBlockBrakesSprites[0][0], &kBlockBrakesOffsets[0], &kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
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
        { &kDiagBrakesSprites[0][0], &kDiagBrakesOffsets[0][0], &kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0], &kDiagBlockBrakesOffsets[0][0], &kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
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
} // namespace OpenRCT2::JuniorRollerCoaster
