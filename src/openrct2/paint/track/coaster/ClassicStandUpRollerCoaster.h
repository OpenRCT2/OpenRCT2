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
#include "../../../paint/track/coaster/StandUpRollerCoaster.h"
#include "../../../ride/RideData.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::ClassicStandUpRollerCoaster
{
    // clang-format off
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        SPR_CSG_BEGIN + 67011,
        SPR_CSG_BEGIN + 67016,
        SPR_CSG_BEGIN + 67010,
        SPR_CSG_BEGIN + 67013,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        SPR_CSG_BEGIN + 67009,
        SPR_CSG_BEGIN + 67014,
        SPR_CSG_BEGIN + 67012,
        SPR_CSG_BEGIN + 67015,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kFlatToRightBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 7>, kNumOrthogonalDirections>
    kBankedRightQuarterTurn5TilesSprites = {{
        {{
            SPR_CSG_BEGIN + 67017,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67018,
            SPR_CSG_BEGIN + 67019,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67020,
            SPR_CSG_BEGIN + 67021,
        }},
        {{
            SPR_CSG_BEGIN + 67022,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67023,
            SPR_CSG_BEGIN + 67024,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67025,
            SPR_CSG_BEGIN + 67026,
        }},
        {{
            SPR_CSG_BEGIN + 67027,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67028,
            SPR_CSG_BEGIN + 67029,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67030,
            SPR_CSG_BEGIN + 67031,
        }},
        {{
            SPR_CSG_BEGIN + 67032,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67033,
            SPR_CSG_BEGIN + 67034,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67035,
            SPR_CSG_BEGIN + 67036,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 7>, kNumOrthogonalDirections>
    kBankedRightQuarterTurn5TilesOffsets = {{
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
    kBankedRightQuarterTurn5TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 32, 3 } },
            { { 0, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 0 }, { 32, 16, 3 } },
            { { 16, 16, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 32, 3 } },
            { { 16, 0, 0 }, { 16, 16, 3 } },
            {},
            { { 0, 16, 0 }, { 32, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        SPR_CSG_BEGIN + 67045,
        SPR_CSG_BEGIN + 67046,
        SPR_CSG_BEGIN + 67047,
        SPR_CSG_BEGIN + 67048,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        SPR_CSG_BEGIN + 67049,
        SPR_CSG_BEGIN + 67050,
        SPR_CSG_BEGIN + 67051,
        SPR_CSG_BEGIN + 67052,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kRightBankToUp25Offsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        SPR_CSG_BEGIN + 67041,
        SPR_CSG_BEGIN + 67042,
        SPR_CSG_BEGIN + 67043,
        SPR_CSG_BEGIN + 67044,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        SPR_CSG_BEGIN + 67037,
        SPR_CSG_BEGIN + 67038,
        SPR_CSG_BEGIN + 67039,
        SPR_CSG_BEGIN + 67040,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        SPR_CSG_BEGIN + 67053,
        SPR_CSG_BEGIN + 67054,
        SPR_CSG_BEGIN + 67055,
        SPR_CSG_BEGIN + 67056,
    }};
    
    static constexpr std::array<CoordsXYZ, kNumOrthogonalDirections>
    kLeftBankOffsets = {{
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
        { 0, 6, 0 }, 
        { 6, 0, 0 }, 
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TilesSprites = {{
        {{
            SPR_CSG_BEGIN + 67229,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67230,
            SPR_CSG_BEGIN + 67231,
        }},
        {{
            SPR_CSG_BEGIN + 67232,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67233,
            SPR_CSG_BEGIN + 67234,
        }},
        {{
            SPR_CSG_BEGIN + 67235,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67236,
            SPR_CSG_BEGIN + 67237,
        }},
        {{
            SPR_CSG_BEGIN + 67238,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67239,
            SPR_CSG_BEGIN + 67240,
        }},
    }};

    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn3TilesOffsets = {{
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
    kRightBankedQuarterTurn3TilesBoundBoxes = {{
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 16, 16, 0 }, { 16, 16, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 16, 0, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
        {{
            { { 0, 6, 0 }, { 32, 20, 3 } },
            {},
            { { 0, 0, 0 }, { 16, 16, 3 } },
            { { 6, 0, 0 }, { 20, 32, 3 } },
        }},
        {{
            { { 6, 0, 0 }, { 20, 32, 3 } },
            {},
            { { 0, 16, 0 }, { 16, 16, 3 } },
            { { 0, 6, 0 }, { 32, 20, 3 } },
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            SPR_CSG_BEGIN + 67377,
            SPR_CSG_BEGIN + 67378,
            SPR_CSG_BEGIN + 67379,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67380,
        }},
        {{
            SPR_CSG_BEGIN + 67381,
            SPR_CSG_BEGIN + 67382,
            SPR_CSG_BEGIN + 67383,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67384,
        }},
        {{
            SPR_CSG_BEGIN + 67385,
            SPR_CSG_BEGIN + 67386,
            SPR_CSG_BEGIN + 67387,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67388,
        }},
        {{
            SPR_CSG_BEGIN + 67389,
            SPR_CSG_BEGIN + 67390,
            SPR_CSG_BEGIN + 67391,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67392,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagBoundBoxes = {{
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
    kRightEighthBankToDiagSprites = {{
        {{
            SPR_CSG_BEGIN + 67361,
            SPR_CSG_BEGIN + 67362,
            SPR_CSG_BEGIN + 67363,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67364,
        }},
        {{
            SPR_CSG_BEGIN + 67365,
            SPR_CSG_BEGIN + 67366,
            SPR_CSG_BEGIN + 67367,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67368,
        }},
        {{
            SPR_CSG_BEGIN + 67369,
            SPR_CSG_BEGIN + 67370,
            SPR_CSG_BEGIN + 67371,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67372,
        }},
        {{
            SPR_CSG_BEGIN + 67373,
            SPR_CSG_BEGIN + 67374,
            SPR_CSG_BEGIN + 67375,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67376,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67425,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67426,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67427,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67428,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankOffsets = {{
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
    kDiagFlatToLeftBankBoundBoxes = {{
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
    kDiagFlatToRightBankSprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67429,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67430,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67431,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67432,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankOffsets = {{
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
    kDiagFlatToRightBankBoundBoxes = {{
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
    kDiagLeftBankToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67441,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67442,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67443,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67444,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Offsets = {{
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
    kDiagLeftBankToUp25BoundBoxes = {{
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
    kDiagRightBankToUp25Sprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67445,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67446,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67447,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67448,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Offsets = {{
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
    kDiagRightBankToUp25BoundBoxes = {{
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
    kDiagUp25ToLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67433,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67434,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67435,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67436,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankOffsets = {{
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
    kDiagUp25ToLeftBankBoundBoxes = {{
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
    kDiagUp25ToRightBankSprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67437,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67438,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67439,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67440,
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
    }};
    
    static constexpr std::array<std::array<CoordsXYZ, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankOffsets = {{
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
    kDiagUp25ToRightBankBoundBoxes = {{
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
    kDiagLeftBankSprites = {{
        {{
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67421,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67422,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_CSG_BEGIN + 67423,
            kImageIndexUndefined,
        }},
        {{
            SPR_CSG_BEGIN + 67424,
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &OpenRCT2::StandUpRollerCoaster::kFlatSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kFlatOffsets[0], &OpenRCT2::StandUpRollerCoaster::kFlatBoundBoxes[0], false }, // Flat
        { &OpenRCT2::StandUpRollerCoaster::kBlockBrakesSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kFlatOffsets[0], &OpenRCT2::StandUpRollerCoaster::kStationBoundBoxes[0], false }, // EndStation
        { &OpenRCT2::StandUpRollerCoaster::kStationSprites[0], &OpenRCT2::StandUpRollerCoaster::kFlatOffsets[0], &OpenRCT2::StandUpRollerCoaster::kStationBoundBoxes[0], false }, // BeginStation
        { &OpenRCT2::StandUpRollerCoaster::kStationSprites[0], &OpenRCT2::StandUpRollerCoaster::kFlatOffsets[0], &OpenRCT2::StandUpRollerCoaster::kStationBoundBoxes[0], false }, // MiddleStation
        { &OpenRCT2::StandUpRollerCoaster::kUp25Sprites[0][0], &OpenRCT2::StandUpRollerCoaster::kUp25Offsets[0], &OpenRCT2::StandUpRollerCoaster::kUp25BoundBoxes[0], false }, // Up25
        { &OpenRCT2::StandUpRollerCoaster::kUp60Sprites[0][0], &OpenRCT2::StandUpRollerCoaster::kUp60Offsets[0], &OpenRCT2::StandUpRollerCoaster::kUp60BoundBoxes[0], false }, // Up60
        { &OpenRCT2::StandUpRollerCoaster::kFlatToUp25Sprites[0][0], &OpenRCT2::StandUpRollerCoaster::kFlatToUp25Offsets[0], &OpenRCT2::StandUpRollerCoaster::kFlatToUp25BoundBoxes[0], false }, // FlatToUp25
        { &OpenRCT2::StandUpRollerCoaster::kUp25ToUp60Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kUp25ToUp60Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &OpenRCT2::StandUpRollerCoaster::kUp60ToUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kUp60ToUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &OpenRCT2::StandUpRollerCoaster::kUp25ToFlatSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kUp25ToFlatOffsets[0], &OpenRCT2::StandUpRollerCoaster::kUp25ToFlatBoundBoxes[0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn5TilesSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn5TilesOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn5TilesBoundBoxes[0][0], false }, // LeftQuarterTurn5Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0], &kFlatToLeftBankOffsets[0], &kFlatToLeftBankBoundBoxes[0], false }, // FlatToLeftBank
        { &kFlatToRightBankSprites[0], &kFlatToRightBankOffsets[0], &kFlatToRightBankBoundBoxes[0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // RightBankToFlat
        { nullptr, nullptr, nullptr, true }, // BankedLeftQuarterTurn5Tiles
        { &kBankedRightQuarterTurn5TilesSprites[0][0], &kBankedRightQuarterTurn5TilesOffsets[0][0], &kBankedRightQuarterTurn5TilesBoundBoxes[0][0], false }, // BankedRightQuarterTurn5Tiles
        { &kLeftBankToUp25Sprites[0], &kLeftBankToUp25Offsets[0], &kLeftBankToUp25BoundBoxes[0], false }, // LeftBankToUp25
        { &kRightBankToUp25Sprites[0], &kRightBankToUp25Offsets[0], &kRightBankToUp25BoundBoxes[0], false }, // RightBankToUp25
        { &kUp25ToLeftBankSprites[0], &kUp25ToLeftBankOffsets[0], &kUp25ToLeftBankBoundBoxes[0], false }, // Up25ToLeftBank
        { &kUp25ToRightBankSprites[0], &kUp25ToRightBankOffsets[0], &kUp25ToRightBankBoundBoxes[0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true }, // LeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // RightBankToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // Down25ToRightBank
        { &kLeftBankSprites[0], &kLeftBankOffsets[0], &kLeftBankBoundBoxes[0], false }, // LeftBank
        { nullptr, nullptr, nullptr, true }, // RightBank
        { &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn5TilesUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn5TilesUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn5TilesUp25
        { &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn5TilesUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn5TilesUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn5TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &OpenRCT2::StandUpRollerCoaster::kSBendLeftSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kSBendLeftOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kSBendLeftBoundBoxes[0][0], false }, // SBendLeft
        { &OpenRCT2::StandUpRollerCoaster::kSBendRightSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kSBendRightOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kSBendRightBoundBoxes[0][0], false }, // SBendRight
        { &OpenRCT2::StandUpRollerCoaster::kLeftVerticalLoopSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftVerticalLoopOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftVerticalLoopBoundBoxes[0][0], false }, // LeftVerticalLoop
        { &OpenRCT2::StandUpRollerCoaster::kRightVerticalLoopSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kRightVerticalLoopOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kRightVerticalLoopBoundBoxes[0][0], false }, // RightVerticalLoop
        { &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn3TilesSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn3TilesOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn3TilesBoundBoxes[0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3Tiles
        { &kRightBankedQuarterTurn3TilesSprites[0][0], &kRightBankedQuarterTurn3TilesOffsets[0][0], &kRightBankedQuarterTurn3TilesBoundBoxes[0][0], false }, // RightBankedQuarterTurn3Tiles
        { &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn3TilesUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn3TilesUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // LeftQuarterTurn3TilesUp25
        { &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn3TilesUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn3TilesUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn3TilesUp25BoundBoxes[0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false }, // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true }, // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true }, // RightTwistUpToDown
        { &OpenRCT2::StandUpRollerCoaster::kHalfLoopUpSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kHalfLoopUpOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kHalfLoopUpBoundBoxes[0][0], false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true }, // HalfLoopDown
        { &OpenRCT2::StandUpRollerCoaster::kLeftCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftCorkscrewUpBoundBoxes[0][0], false }, // LeftCorkscrewUp
        { &OpenRCT2::StandUpRollerCoaster::kRightCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightCorkscrewUpBoundBoxes[0][0], false }, // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightCorkscrewDown
        { &OpenRCT2::StandUpRollerCoaster::kFlatToUp60Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kFlatToUp60BoundBoxes[0][0], false }, // FlatToUp60
        { &OpenRCT2::StandUpRollerCoaster::kUp60ToFlatSprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kUp60ToFlatBoundBoxes[0][0], false }, // Up60ToFlat
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
        { &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn1TileUp60Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn1TileUp60BoundBoxes[0][0], false }, // LeftQuarterTurn1TileUp60
        { &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn1TileUp60Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn1TileUp60BoundBoxes[0][0], false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &OpenRCT2::StandUpRollerCoaster::kBrakesSprites[0], &OpenRCT2::StandUpRollerCoaster::kBrakesOffsets[0], &OpenRCT2::StandUpRollerCoaster::kBrakesBoundBoxes[0], false }, // Brakes
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
        { &OpenRCT2::StandUpRollerCoaster::kFlatSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kFlatOffsets[0], &OpenRCT2::StandUpRollerCoaster::kStationBoundBoxes[0], false }, // OnRidePhoto
        { nullptr, nullptr, nullptr, true }, // Down25LeftBanked
        { nullptr, nullptr, nullptr, true }, // Down25RightBanked
        { nullptr, nullptr, nullptr, false }, // Watersplash
        { &OpenRCT2::StandUpRollerCoaster::kFlatToUp60LongBaseSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kFlatToUp60LongBaseBoundBoxes[0][0][0], false }, // FlatToUp60LongBase
        { &OpenRCT2::StandUpRollerCoaster::kUp60ToFlatLongBaseSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kUp60ToFlatLongBaseBoundBoxes[0][0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // Whirlpool
        { nullptr, nullptr, nullptr, true }, // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false }, // CableLiftHill
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallVertical
        { &OpenRCT2::StandUpRollerCoaster::kUp90Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kUp90BoundBoxes[0][0], false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { &OpenRCT2::StandUpRollerCoaster::kUp60ToUp90Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kUp60ToUp90BoundBoxes[0][0], false }, // Up60ToUp90
        { &OpenRCT2::StandUpRollerCoaster::kDown90ToDown60Sprites[0], nullptr, &OpenRCT2::StandUpRollerCoaster::kDown90ToDown60BoundBoxes[0], false }, // Down90ToDown60
        { &OpenRCT2::StandUpRollerCoaster::kUp90ToUp60Sprites[0], nullptr, &OpenRCT2::StandUpRollerCoaster::kUp90ToUp60BoundBoxes[0], false }, // Up90ToUp60
        { &OpenRCT2::StandUpRollerCoaster::kDown60ToDown90Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kDown60ToDown90BoundBoxes[0][0], false }, // Down60ToDown90
        { nullptr, nullptr, nullptr, false }, // BrakeForDrop
        { &OpenRCT2::StandUpRollerCoaster::kLeftEighthToDiagSprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftEighthToDiagBoundBoxes[0][0], false }, // LeftEighthToDiag
        { &OpenRCT2::StandUpRollerCoaster::kRightEighthToDiagSprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightEighthToDiagBoundBoxes[0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { &kLeftEighthBankToDiagSprites[0][0], nullptr, &kLeftEighthBankToDiagBoundBoxes[0][0], false }, // LeftEighthBankToDiag
        { &kRightEighthBankToDiagSprites[0][0], nullptr, &kRightEighthBankToDiagBoundBoxes[0][0], false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &OpenRCT2::StandUpRollerCoaster::kDiagFlatSprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagFlatOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagFlatBoundBoxes[0][0], false }, // DiagFlat
        { &OpenRCT2::StandUpRollerCoaster::kDiagUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp25BoundBoxes[0][0], false }, // DiagUp25
        { &OpenRCT2::StandUpRollerCoaster::kDiagUp60Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp60Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp60BoundBoxes[0][0], false }, // DiagUp60
        { &OpenRCT2::StandUpRollerCoaster::kDiagFlatToUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagFlatToUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagFlatToUp25BoundBoxes[0][0], false }, // DiagFlatToUp25
        { &OpenRCT2::StandUpRollerCoaster::kDiagUp25ToUp60Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp25ToUp60Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp25ToUp60BoundBoxes[0][0], false }, // DiagUp25ToUp60
        { &OpenRCT2::StandUpRollerCoaster::kDiagUp60ToUp25Sprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp60ToUp25Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp60ToUp25BoundBoxes[0][0], false }, // DiagUp60ToUp25
        { &OpenRCT2::StandUpRollerCoaster::kDiagUp25ToFlatSprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp25ToFlatOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp25ToFlatBoundBoxes[0][0], false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown60
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToFlat
        { &OpenRCT2::StandUpRollerCoaster::kDiagFlatToUp60Sprites[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagFlatToUp60Offsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagFlatToUp60BoundBoxes[0][0], false }, // DiagFlatToUp60
        { &OpenRCT2::StandUpRollerCoaster::kDiagUp60ToFlatSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp60ToFlatOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagUp60ToFlatBoundBoxes[0][0], false }, // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true }, // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true }, // DiagDown60ToFlat
        { &kDiagFlatToLeftBankSprites[0][0], &kDiagFlatToLeftBankOffsets[0][0], &kDiagFlatToLeftBankBoundBoxes[0][0], false }, // DiagFlatToLeftBank
        { &kDiagFlatToRightBankSprites[0][0], &kDiagFlatToRightBankOffsets[0][0], &kDiagFlatToRightBankBoundBoxes[0][0], false }, // DiagFlatToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToFlat
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToFlat
        { &kDiagLeftBankToUp25Sprites[0][0], &kDiagLeftBankToUp25Offsets[0][0], &kDiagLeftBankToUp25BoundBoxes[0][0], false }, // DiagLeftBankToUp25
        { &kDiagRightBankToUp25Sprites[0][0], &kDiagRightBankToUp25Offsets[0][0], &kDiagRightBankToUp25BoundBoxes[0][0], false }, // DiagRightBankToUp25
        { &kDiagUp25ToLeftBankSprites[0][0], &kDiagUp25ToLeftBankOffsets[0][0], &kDiagUp25ToLeftBankBoundBoxes[0][0], false }, // DiagUp25ToLeftBank
        { &kDiagUp25ToRightBankSprites[0][0], &kDiagUp25ToRightBankOffsets[0][0], &kDiagUp25ToRightBankBoundBoxes[0][0], false }, // DiagUp25ToRightBank
        { nullptr, nullptr, nullptr, true }, // DiagLeftBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagRightBankToDown25
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagDown25ToRightBank
        { &kDiagLeftBankSprites[0][0], &kDiagLeftBankOffsets[0][0], &kDiagLeftBankBoundBoxes[0][0], false }, // DiagLeftBank
        { nullptr, nullptr, nullptr, true }, // DiagRightBank
        { nullptr, nullptr, nullptr, false }, // LogFlumeReverser
        { nullptr, nullptr, nullptr, false }, // SpinningTunnel
        { &OpenRCT2::StandUpRollerCoaster::kLeftBarrelRollUpToDownSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftBarrelRollUpToDownBoundBoxes[0][0][0], false }, // LeftBarrelRollUpToDown
        { &OpenRCT2::StandUpRollerCoaster::kRightBarrelRollUpToDownSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightBarrelRollUpToDownBoundBoxes[0][0][0], false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true }, // RightBarrelRollDownToUp
        { nullptr, nullptr, nullptr, false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false }, // PoweredLift
        { &OpenRCT2::StandUpRollerCoaster::kLeftLargeHalfLoopUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftLargeHalfLoopUpBoundBoxes[0][0][0], false }, // LeftLargeHalfLoopUp
        { &OpenRCT2::StandUpRollerCoaster::kRightLargeHalfLoopUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightLargeHalfLoopUpBoundBoxes[0][0][0], false }, // RightLargeHalfLoopUp
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
        { &OpenRCT2::StandUpRollerCoaster::kUp90ToInvertedFlatQuarterLoopSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &OpenRCT2::StandUpRollerCoaster::kInvertedFlatToDown90QuarterLoopSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0][0], false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &OpenRCT2::StandUpRollerCoaster::kBlockBrakesSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kBlockBrakesOffsets[0], &OpenRCT2::StandUpRollerCoaster::kBlockBrakesBoundBoxes[0], false }, // BlockBrakes
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
        { &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // LeftQuarterTurn1TileUp90
        { &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // RightQuarterTurn1TileUp90
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
        { &OpenRCT2::StandUpRollerCoaster::kLeftLargeCorkscrewUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftLargeCorkscrewUpBoundBoxes[0][0][0], false }, // LeftLargeCorkscrewUp
        { &OpenRCT2::StandUpRollerCoaster::kRightLargeCorkscrewUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightLargeCorkscrewUpBoundBoxes[0][0][0], false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { &OpenRCT2::StandUpRollerCoaster::kLeftMediumHalfLoopUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftMediumHalfLoopUpBoundBoxes[0][0][0], false }, // LeftMediumHalfLoopUp
        { &OpenRCT2::StandUpRollerCoaster::kRightMediumHalfLoopUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightMediumHalfLoopUpBoundBoxes[0][0][0], false }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightMediumHalfLoopDown
        { &OpenRCT2::StandUpRollerCoaster::kLeftZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftZeroGRollUpBoundBoxes[0][0][0], false }, // LeftZeroGRollUp
        { &OpenRCT2::StandUpRollerCoaster::kRightZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightZeroGRollUpBoundBoxes[0][0][0], false }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true }, // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true }, // RightZeroGRollDown
        { &OpenRCT2::StandUpRollerCoaster::kLeftLargeZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftLargeZeroGRollUpBoundBoxes[0][0][0], false }, // LeftLargeZeroGRollUp
        { &OpenRCT2::StandUpRollerCoaster::kRightLargeZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightLargeZeroGRollUpBoundBoxes[0][0][0], false }, // RightLargeZeroGRollUp
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
        { &OpenRCT2::StandUpRollerCoaster::kLeftEighthToDiagUp25Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftEighthToDiagUp25BoundBoxes[0][0], false }, // LeftEighthToDiagUp25
        { &OpenRCT2::StandUpRollerCoaster::kRightEighthToDiagUp25Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightEighthToDiagUp25BoundBoxes[0][0], false }, // RightEighthToDiagUp25
        { nullptr, nullptr, nullptr, true }, // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true }, // RightEighthToDiagDown25
        { &OpenRCT2::StandUpRollerCoaster::kLeftEighthToOrthogonalUp25Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kLeftEighthToOrthogonalUp25BoundBoxes[0][0], false }, // LeftEighthToOrthogonalUp25
        { &OpenRCT2::StandUpRollerCoaster::kRightEighthToOrthogonalUp25Sprites[0][0], nullptr, &OpenRCT2::StandUpRollerCoaster::kRightEighthToOrthogonalUp25BoundBoxes[0][0], false }, // RightEighthToOrthogonalUp25
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
        { &OpenRCT2::StandUpRollerCoaster::kDiagBrakesSprites[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagBrakesOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagBrakesBoundBoxes[0][0], false }, // DiagBrakes
        { &OpenRCT2::StandUpRollerCoaster::kDiagBlockBrakesSprites[0][0][0], &OpenRCT2::StandUpRollerCoaster::kDiagBlockBrakesOffsets[0][0], &OpenRCT2::StandUpRollerCoaster::kDiagBlockBrakesBoundBoxes[0][0], false }, // DiagBlockBrakes
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
} // namespace OpenRCT2::ClassicStandUpRollerCoaster
