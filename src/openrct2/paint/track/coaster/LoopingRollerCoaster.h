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
#include "../../../paint/track/coaster/LimLaunchedRollerCoasterShared.h"
#include "../../../ride/RideData.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::LoopingRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            15004,
            15005,
            15004,
            15005,
        }},
        {{
            15006,
            15007,
            15008,
            15009,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
        { { 0, 6, 1 }, { 32, 20, 2 } },
        { { 6, 0, 1 }, { 20, 32, 2 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            15032,
            15033,
            15034,
            15035,
        }},
        {{
            15060,
            15061,
            15062,
            15063,
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
            15048,
            15049,
            15050,
            15051,
        }},
        {{
            15076,
            15077,
            15078,
            15079,
        }},
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        { { 0, 6, 0 }, { 32, 20, 3 } },
        { { 27, 0, 0 }, { 1, 32, 98 } },
        { { 0, 27, 0 }, { 32, 1, 98 } },
        { { 6, 0, 0 }, { 20, 32, 3 } },
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            15024,
            15025,
            15026,
            15027,
        }},
        {{
            15052,
            15053,
            15054,
            15055,
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
            {{ 15036, kImageIndexUndefined, }},
            {{ 15037, 15040, }},
            {{ 15038, 15041, }},
            {{ 15039, kImageIndexUndefined, }},
        }},
        {{
            {{ 15064, kImageIndexUndefined, }},
            {{ 15065, 15068, }},
            {{ 15066, 15069, }},
            {{ 15067, kImageIndexUndefined, }},
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
            {{ 15042, kImageIndexUndefined, }},
            {{ 15043, 15046, }},
            {{ 15044, 15047, }},
            {{ 15045, kImageIndexUndefined, }},
        }},
        {{
            {{ 15070, kImageIndexUndefined, }},
            {{ 15071, 15074, }},
            {{ 15072, 15075, }},
            {{ 15073, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 66 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 66 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            15028,
            15029,
            15030,
            15031,
        }},
        {{
            15056,
            15057,
            15058,
            15059,
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
            15183,
            kImageIndexUndefined,
            15182,
            15181,
            kImageIndexUndefined,
            15180,
            15179,
        }},
        {{
            15188,
            kImageIndexUndefined,
            15187,
            15186,
            kImageIndexUndefined,
            15185,
            15184,
        }},
        {{
            15193,
            kImageIndexUndefined,
            15192,
            15191,
            kImageIndexUndefined,
            15190,
            15189,
        }},
        {{
            15178,
            kImageIndexUndefined,
            15177,
            15176,
            kImageIndexUndefined,
            15175,
            15174,
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
        {{ 15080, 15092, }},
        {{ 15081, 15093, }},
        {{ 15082, kImageIndexUndefined, }},
        {{ 15083, kImageIndexUndefined, }},
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
        {{ 15084, kImageIndexUndefined, }},
        {{ 15085, kImageIndexUndefined, }},
        {{ 15086, 15094, }},
        {{ 15087, 15095, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 15203, 15214, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15202, kImageIndexUndefined, }},
            {{ 15201, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15200, kImageIndexUndefined, }},
            {{ 15199, kImageIndexUndefined, }},
        }},
        {{
            {{ 15208, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15207, kImageIndexUndefined, }},
            {{ 15206, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15205, kImageIndexUndefined, }},
            {{ 15204, kImageIndexUndefined, }},
        }},
        {{
            {{ 15213, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15212, kImageIndexUndefined, }},
            {{ 15211, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15210, kImageIndexUndefined, }},
            {{ 15209, 15215, }},
        }},
        {{
            {{ 15198, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15197, kImageIndexUndefined, }},
            {{ 15196, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15195, kImageIndexUndefined, }},
            {{ 15194, kImageIndexUndefined, }},
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
        {{ 15096, 15112, }},
        {{ 15097, 15113, }},
        {{ 15098, kImageIndexUndefined, }},
        {{ 15099, kImageIndexUndefined, }},
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
        {{ 15100, kImageIndexUndefined, }},
        {{ 15101, kImageIndexUndefined, }},
        {{ 15102, 15114, }},
        {{ 15103, 15115, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 15104, 15116, }},
        {{ 15105, 15117, }},
        {{ 15106, kImageIndexUndefined, }},
        {{ 15107, kImageIndexUndefined, }},
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
        {{ 15108, kImageIndexUndefined, }},
        {{ 15109, kImageIndexUndefined, }},
        {{ 15110, 15118, }},
        {{ 15111, 15119, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        15088,
        15089,
        15090,
        15091,
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
            15296,
            kImageIndexUndefined,
            15297,
            15298,
            kImageIndexUndefined,
            15299,
            15300,
        }},
        {{
            15301,
            kImageIndexUndefined,
            15302,
            15303,
            kImageIndexUndefined,
            15304,
            15305,
        }},
        {{
            15306,
            kImageIndexUndefined,
            15307,
            15308,
            kImageIndexUndefined,
            15309,
            15310,
        }},
        {{
            15311,
            kImageIndexUndefined,
            15312,
            15313,
            kImageIndexUndefined,
            15314,
            15315,
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
            15276,
            kImageIndexUndefined,
            15277,
            15278,
            kImageIndexUndefined,
            15279,
            15280,
        }},
        {{
            15281,
            kImageIndexUndefined,
            15282,
            15283,
            kImageIndexUndefined,
            15284,
            15285,
        }},
        {{
            15286,
            kImageIndexUndefined,
            15287,
            15288,
            kImageIndexUndefined,
            15289,
            15290,
        }},
        {{
            15291,
            kImageIndexUndefined,
            15292,
            15293,
            kImageIndexUndefined,
            15294,
            15295,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightQuarterTurn5TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn5TilesUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            15260,
            15261,
            15262,
            15263,
        }},
        {{
            15264,
            15265,
            15266,
            15267,
        }},
        {{
            15263,
            15262,
            15261,
            15260,
        }},
        {{
            15267,
            15266,
            15265,
            15264,
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
            15268,
            15269,
            15270,
            15271,
        }},
        {{
            15272,
            15273,
            15274,
            15275,
        }},
        {{
            15271,
            15270,
            15269,
            15268,
        }},
        {{
            15275,
            15274,
            15273,
            15272,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopSprites = {{
        {{
            {{ 15348, kImageIndexUndefined, }},
            {{ 15349, kImageIndexUndefined, }},
            {{ 15350, 15364, }},
            {{ 15351, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15352, kImageIndexUndefined, }},
            {{ 15353, 15365, }},
            {{ 15354, kImageIndexUndefined, }},
            {{ 15355, kImageIndexUndefined, }},
        }},
        {{
            {{ 15356, kImageIndexUndefined, }},
            {{ 15357, kImageIndexUndefined, }},
            {{ 15358, 15366, }},
            {{ 15359, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15360, kImageIndexUndefined, }},
            {{ 15361, 15367, }},
            {{ 15362, kImageIndexUndefined, }},
            {{ 15363, kImageIndexUndefined, }},
        }},
        {{
            {{ 15355, kImageIndexUndefined, }},
            {{ 15354, kImageIndexUndefined, }},
            {{ 15353, 15365, }},
            {{ 15352, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15351, kImageIndexUndefined, }},
            {{ 15350, 15364, }},
            {{ 15349, kImageIndexUndefined, }},
            {{ 15348, kImageIndexUndefined, }},
        }},
        {{
            {{ 15363, kImageIndexUndefined, }},
            {{ 15362, kImageIndexUndefined, }},
            {{ 15361, 15367, }},
            {{ 15360, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15359, kImageIndexUndefined, }},
            {{ 15358, 15366, }},
            {{ 15357, kImageIndexUndefined, }},
            {{ 15356, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopOffsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ { 0, 0, 0 }, {} }},
            {{ { 16, 0, 0 }, { 16, 0, 0 } }},
            {{ { 0, 0, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 16, 32 }, {} }},
            {{ { 10, 16, 0 }, { 10, 16, 0 } }},
            {{ { 0, 6, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ { 14, 0, 0 }, {} }},
            {{ { 0, 12, 0 }, { 0, 12, 0 } }},
            {{ { 0, 0, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 16, 0, 32 }, {} }},
            {{ { 16, 16, 0 }, { 16, 16, 0 } }},
            {{ { 6, 0, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
            {{ { 10, 16, 0 }, { 10, 16, 0 } }},
            {{ { 0, 16, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 0, 32 }, {} }},
            {{ { 16, 0, 0 }, { 16, 0, 0 } }},
            {{ { 0, 0, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
            {{ { 16, 16, 0 }, { 16, 16, 0 } }},
            {{ { 16, 0, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 0, 32 }, {} }},
            {{ { 0, 12, 0 }, { 0, 12, 0 } }},
            {{ { 14, 0, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 10>, kNumOrthogonalDirections>
    kLeftVerticalLoopBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 0, 0 }, { 32, 26, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 3, 16, 119 } }, { { 16, 0, 0 }, { 3, 16, 119 } } }},
            {{ { { 0, 0, 32 }, { 32, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 16, 32 }, { 32, 16, 3 } }, {} }},
            {{ { { 10, 16, 0 }, { 4, 16, 119 } }, { { 10, 16, 0 }, { 4, 16, 119 } } }},
            {{ { { 0, 6, 0 }, { 32, 26, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
            {{ { { 14, 0, 0 }, { 2, 32, 63 } }, {} }},
            {{ { { 0, 12, 0 }, { 16, 3, 119 } }, { { 0, 12, 0 }, { 16, 3, 119 } } }},
            {{ { { 0, 0, 32 }, { 16, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 16, 0, 32 }, { 16, 32, 3 } }, {} }},
            {{ { { 16, 16, 0 }, { 16, 2, 119 } }, { { 16, 16, 0 }, { 16, 2, 119 } } }},
            {{ { { 6, 0, 0 }, { 26, 32, 3 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 7 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 26, 3 } }, {} }},
            {{ { { 10, 16, 0 }, { 4, 16, 119 } }, { { 10, 16, 0 }, { 4, 16, 119 } } }},
            {{ { { 0, 16, 32 }, { 32, 16, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 32 }, { 32, 16, 3 } }, {} }},
            {{ { { 16, 0, 0 }, { 3, 16, 119 } }, { { 16, 0, 0 }, { 3, 16, 119 } } }},
            {{ { { 0, 0, 0 }, { 32, 26, 3 } }, {} }},
            {{ { { 0, 6, 0 }, { 32, 20, 3 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 7 } }, {} }},
            {{ { { 6, 0, 0 }, { 26, 32, 3 } }, {} }},
            {{ { { 16, 16, 0 }, { 16, 2, 119 } }, { { 16, 16, 0 }, { 16, 2, 119 } } }},
            {{ { { 16, 0, 32 }, { 16, 32, 3 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 32 }, { 16, 32, 3 } }, {} }},
            {{ { { 0, 12, 0 }, { 16, 3, 119 } }, { { 0, 12, 0 }, { 16, 3, 119 } } }},
            {{ { { 14, 0, 0 }, { 2, 32, 63 } }, {} }},
            {{ { { 6, 0, 0 }, { 20, 32, 3 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopSprites = {{
        {{
            {{ 15383, kImageIndexUndefined, }},
            {{ 15382, kImageIndexUndefined, }},
            {{ 15381, 15384, }},
            {{ 15380, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15379, kImageIndexUndefined, }},
            {{ 15378, 15385, }},
            {{ 15377, kImageIndexUndefined, }},
            {{ 15376, kImageIndexUndefined, }},
        }},
        {{
            {{ 15375, kImageIndexUndefined, }},
            {{ 15374, kImageIndexUndefined, }},
            {{ 15373, 15386, }},
            {{ 15372, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15371, kImageIndexUndefined, }},
            {{ 15370, 15387, }},
            {{ 15369, kImageIndexUndefined, }},
            {{ 15368, kImageIndexUndefined, }},
        }},
        {{
            {{ 15376, kImageIndexUndefined, }},
            {{ 15377, kImageIndexUndefined, }},
            {{ 15378, 15385, }},
            {{ 15379, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15380, kImageIndexUndefined, }},
            {{ 15381, 15384, }},
            {{ 15382, kImageIndexUndefined, }},
            {{ 15383, kImageIndexUndefined, }},
        }},
        {{
            {{ 15368, kImageIndexUndefined, }},
            {{ 15369, kImageIndexUndefined, }},
            {{ 15370, 15387, }},
            {{ 15371, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15372, kImageIndexUndefined, }},
            {{ 15373, 15386, }},
            {{ 15374, kImageIndexUndefined, }},
            {{ 15375, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopOffsets = {{
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
            {{ { 16, 16, 0 }, { 16, 16, 0 } }},
            {{ { 0, 16, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 0, 32 }, {} }},
            {{ { 12, 0, 0 }, { 12, 0, 0 } }},
            {{ { 0, 14, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
            {{ { 16, 10, 0 }, { 16, 10, 0 } }},
            {{ { 16, 0, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 0, 32 }, {} }},
            {{ { 0, 16, 0 }, { 0, 16, 0 } }},
            {{ { 0, 0, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
        {{
            {{ { 0, 6, 0 }, {} }},
            {{ { 0, 14, 0 }, {} }},
            {{ { 12, 0, 0 }, { 12, 0, 0 } }},
            {{ { 0, 0, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 0, 16, 32 }, {} }},
            {{ { 16, 16, 0 }, { 16, 16, 0 } }},
            {{ { 0, 6, 0 }, {} }},
            {{ { 0, 6, 0 }, {} }},
        }},
        {{
            {{ { 6, 0, 0 }, {} }},
            {{ { 0, 0, 0 }, {} }},
            {{ { 0, 16, 0 }, { 0, 16, 0 } }},
            {{ { 0, 0, 32 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { 16, 0, 32 }, {} }},
            {{ { 16, 10, 0 }, { 16, 10, 0 } }},
            {{ { 6, 0, 0 }, {} }},
            {{ { 6, 0, 0 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 10>, kNumOrthogonalDirections>
    kRightVerticalLoopBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftVerticalLoopBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            15125,
            kImageIndexUndefined,
            15124,
            15123,
        }},
        {{
            15128,
            kImageIndexUndefined,
            15127,
            15126,
        }},
        {{
            15131,
            kImageIndexUndefined,
            15130,
            15129,
        }},
        {{
            15122,
            kImageIndexUndefined,
            15121,
            15120,
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
            {{ 15137, 15144, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15136, kImageIndexUndefined, }},
            {{ 15135, kImageIndexUndefined, }},
        }},
        {{
            {{ 15140, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15139, kImageIndexUndefined, }},
            {{ 15138, kImageIndexUndefined, }},
        }},
        {{
            {{ 15143, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15142, kImageIndexUndefined, }},
            {{ 15141, 15145, }},
        }},
        {{
            {{ 15134, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15133, kImageIndexUndefined, }},
            {{ 15132, kImageIndexUndefined, }},
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
            15327,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15326,
        }},
        {{
            15329,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15328,
        }},
        {{
            15331,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15330,
        }},
        {{
            15325,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15324,
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
            15316,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15317,
        }},
        {{
            15318,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15319,
        }},
        {{
            15320,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15321,
        }},
        {{
            15322,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15323,
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
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToUp60Sprites = {{
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 0, kImageIndexUndefined, }},
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 2, SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 1, }},
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 4, SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 3, }},
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 5, kImageIndexUndefined, }},
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
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 6, kImageIndexUndefined, }},
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 7, SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 8, }},
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 10, SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 9, }},
        {{ SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 11, kImageIndexUndefined, }},
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
            {{ 15165, 15172, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15164, kImageIndexUndefined, }},
            {{ 15163, kImageIndexUndefined, }},
            {{ 15162, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15161, kImageIndexUndefined, }},
            {{ 15160, kImageIndexUndefined, }},
        }},
        {{
            {{ 15168, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15167, kImageIndexUndefined, }},
            {{ 15166, kImageIndexUndefined, }},
            {{ 15165, 15172, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15164, kImageIndexUndefined, }},
            {{ 15163, kImageIndexUndefined, }},
        }},
        {{
            {{ 15171, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15170, kImageIndexUndefined, }},
            {{ 15169, 15173, }},
            {{ 15168, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15167, kImageIndexUndefined, }},
            {{ 15166, kImageIndexUndefined, }},
        }},
        {{
            {{ 15162, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15161, kImageIndexUndefined, }},
            {{ 15160, kImageIndexUndefined, }},
            {{ 15171, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15170, kImageIndexUndefined, }},
            {{ 15169, 15173, }},
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
            {{ 15146, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15147, kImageIndexUndefined, }},
            {{ 15148, kImageIndexUndefined, }},
            {{ 15149, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15150, kImageIndexUndefined, }},
            {{ 15151, 15158, }},
        }},
        {{
            {{ 15149, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15150, kImageIndexUndefined, }},
            {{ 15151, 15158, }},
            {{ 15152, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15153, kImageIndexUndefined, }},
            {{ 15154, kImageIndexUndefined, }},
        }},
        {{
            {{ 15152, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15153, kImageIndexUndefined, }},
            {{ 15154, kImageIndexUndefined, }},
            {{ 15155, 15159, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15156, kImageIndexUndefined, }},
            {{ 15157, kImageIndexUndefined, }},
        }},
        {{
            {{ 15155, 15159, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15156, kImageIndexUndefined, }},
            {{ 15157, kImageIndexUndefined, }},
            {{ 15146, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15147, kImageIndexUndefined, }},
            {{ 15148, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 15247, 15258, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15246, kImageIndexUndefined, }},
            {{ 15245, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15244, kImageIndexUndefined, }},
            {{ 15243, kImageIndexUndefined, }},
            {{ 15242, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15241, kImageIndexUndefined, }},
            {{ 15240, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15239, kImageIndexUndefined, }},
            {{ 15238, kImageIndexUndefined, }},
        }},
        {{
            {{ 15252, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15251, kImageIndexUndefined, }},
            {{ 15250, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15249, kImageIndexUndefined, }},
            {{ 15248, kImageIndexUndefined, }},
            {{ 15247, 15258, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15246, kImageIndexUndefined, }},
            {{ 15245, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15244, kImageIndexUndefined, }},
            {{ 15243, kImageIndexUndefined, }},
        }},
        {{
            {{ 15257, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15256, kImageIndexUndefined, }},
            {{ 15255, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15254, kImageIndexUndefined, }},
            {{ 15253, 15259, }},
            {{ 15252, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15251, kImageIndexUndefined, }},
            {{ 15250, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15249, kImageIndexUndefined, }},
            {{ 15248, kImageIndexUndefined, }},
        }},
        {{
            {{ 15242, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15241, kImageIndexUndefined, }},
            {{ 15240, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15239, kImageIndexUndefined, }},
            {{ 15238, kImageIndexUndefined, }},
            {{ 15257, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15256, kImageIndexUndefined, }},
            {{ 15255, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15254, kImageIndexUndefined, }},
            {{ 15253, 15259, }},
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
            {{ 15216, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15217, kImageIndexUndefined, }},
            {{ 15218, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15219, kImageIndexUndefined, }},
            {{ 15220, kImageIndexUndefined, }},
            {{ 15221, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15222, kImageIndexUndefined, }},
            {{ 15223, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15224, kImageIndexUndefined, }},
            {{ 15225, 15236, }},
        }},
        {{
            {{ 15221, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15222, kImageIndexUndefined, }},
            {{ 15223, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15224, kImageIndexUndefined, }},
            {{ 15225, 15236, }},
            {{ 15226, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15227, kImageIndexUndefined, }},
            {{ 15228, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15229, kImageIndexUndefined, }},
            {{ 15230, kImageIndexUndefined, }},
        }},
        {{
            {{ 15226, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15227, kImageIndexUndefined, }},
            {{ 15228, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15229, kImageIndexUndefined, }},
            {{ 15230, kImageIndexUndefined, }},
            {{ 15231, 15237, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15232, kImageIndexUndefined, }},
            {{ 15233, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15234, kImageIndexUndefined, }},
            {{ 15235, kImageIndexUndefined, }},
        }},
        {{
            {{ 15231, 15237, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15232, kImageIndexUndefined, }},
            {{ 15233, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15234, kImageIndexUndefined, }},
            {{ 15235, kImageIndexUndefined, }},
            {{ 15216, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15217, kImageIndexUndefined, }},
            {{ 15218, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15219, kImageIndexUndefined, }},
            {{ 15220, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60Sprites = {{
        {{ 15341, 15345, }},
        {{ 15342, 15346, }},
        {{ 15343, 15347, }},
        {{ 15340, 15344, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftQuarterTurn1TileUp60BoundBoxes = {{
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 75 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
        {{ { { 2, 2, 0 }, { 28, 28, 3 } }, { { 2, 2, 99 }, { 28, 28, 1 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60Sprites = {{
        {{ 15332, 15336, }},
        {{ 15333, 15337, }},
        {{ 15334, 15338, }},
        {{ 15335, 15339, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightQuarterTurn1TileUp60BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftQuarterTurn1TileUp60BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBrakesSprites = {{
        {{ 15012, 15014, }},
        {{ 15013, 15015, }},
        {{ 15012, 15014, }},
        {{ 15013, 15015, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kBoosterSprites = {{
        15010,
        15011,
        15010,
        15011,
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
        15594,
        15595,
        15596,
        15597,
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
        15598,
        15599,
        15600,
        15601,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kUp25RightBankedBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25LeftBankedBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToUp60LongBaseSprites = {{
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 0,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 1,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 2,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 3,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 4,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 5,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 6,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 7,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 8,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 9,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 10,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 11,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 12,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 13,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 14,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 15,
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
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 16,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 17,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 18,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 19,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 20,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 21,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 22,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 23,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 24,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 25,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 26,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 27,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 28,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 29,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 30,
            SPR_G2_LIM_LAUNCHED_TRACK_FLAT_TO_STEEP + 31,
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
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            15526,
            15527,
            15528,
            kImageIndexUndefined,
            15529,
        }},
        {{
            15530,
            15531,
            15532,
            kImageIndexUndefined,
            15533,
        }},
        {{
            15534,
            15535,
            15536,
            kImageIndexUndefined,
            15537,
        }},
        {{
            15538,
            15539,
            15540,
            kImageIndexUndefined,
            15541,
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
            15510,
            15511,
            15512,
            kImageIndexUndefined,
            15513,
        }},
        {{
            15514,
            15515,
            15516,
            kImageIndexUndefined,
            15517,
        }},
        {{
            15518,
            15519,
            15520,
            kImageIndexUndefined,
            15521,
        }},
        {{
            15522,
            15523,
            15524,
            kImageIndexUndefined,
            15525,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            15558,
            15559,
            15560,
            kImageIndexUndefined,
            15561,
        }},
        {{
            15562,
            15563,
            15564,
            kImageIndexUndefined,
            15565,
        }},
        {{
            15566,
            15567,
            15568,
            kImageIndexUndefined,
            15569,
        }},
        {{
            15570,
            15571,
            15572,
            kImageIndexUndefined,
            15573,
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
            15542,
            15543,
            15544,
            kImageIndexUndefined,
            15545,
        }},
        {{
            15546,
            15547,
            15548,
            kImageIndexUndefined,
            15549,
        }},
        {{
            15550,
            15551,
            15552,
            kImageIndexUndefined,
            15553,
        }},
        {{
            15554,
            15555,
            15556,
            kImageIndexUndefined,
            15557,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                kImageIndexUndefined,
                15420,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15421,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15422,
                kImageIndexUndefined,
            }},
            {{
                15423,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15448,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15449,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15450,
                kImageIndexUndefined,
            }},
            {{
                15451,
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
                15432,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15433,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15434,
                kImageIndexUndefined,
            }},
            {{
                15435,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15460,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15461,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15462,
                kImageIndexUndefined,
            }},
            {{
                15463,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                15444,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15445,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15446,
                kImageIndexUndefined,
            }},
            {{
                15447,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15472,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15473,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15474,
                kImageIndexUndefined,
            }},
            {{
                15475,
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
                15424,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15425,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15426,
                kImageIndexUndefined,
            }},
            {{
                15427,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15452,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15453,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15454,
                kImageIndexUndefined,
            }},
            {{
                15455,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25ToUp60Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                15436,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15437,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15438,
                kImageIndexUndefined,
            }},
            {{
                15439,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15464,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15465,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15466,
                kImageIndexUndefined,
            }},
            {{
                15467,
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
    kDiagUp60ToUp25Sprites = {{
        {{
            {{
                kImageIndexUndefined,
                15440,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15441,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15442,
                kImageIndexUndefined,
            }},
            {{
                15443,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15468,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15469,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15470,
                kImageIndexUndefined,
            }},
            {{
                15471,
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
                15428,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15429,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15430,
                kImageIndexUndefined,
            }},
            {{
                15431,
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
        }},
        {{
            {{
                kImageIndexUndefined,
                15456,
                kImageIndexUndefined,
                kImageIndexUndefined,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                kImageIndexUndefined,
                15457,
            }},
            {{
                kImageIndexUndefined,
                kImageIndexUndefined,
                15458,
                kImageIndexUndefined,
            }},
            {{
                15459,
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
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 12,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 13,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 14,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 15,
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
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 16,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 17,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 18,
            kImageIndexUndefined,
        }},
        {{
            SPR_G2_LIM_LAUNCHED_TRACK_SMALL_FLAT_TO_STEEP + 19,
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
            {{ 15500, 15504, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15501, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15502, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15503, kImageIndexUndefined, }},
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
            {{ 15505, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15506, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15507, 15509, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15508, kImageIndexUndefined, }},
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
            {{ 15490, 15494, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15491, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15492, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15493, kImageIndexUndefined, }},
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
            {{ 15495, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15496, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15497, 15499, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15498, kImageIndexUndefined, }},
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
            {{ 15480, 15484, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15481, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15482, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15483, kImageIndexUndefined, }},
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
            {{ 15485, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15486, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 15487, 15489, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 15488, kImageIndexUndefined, }},
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
            15476,
            kImageIndexUndefined,
            kImageIndexUndefined,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15477,
        }},
        {{
            kImageIndexUndefined,
            kImageIndexUndefined,
            15478,
            kImageIndexUndefined,
        }},
        {{
            15479,
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 0, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 1, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 2, SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 4, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 5, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 6, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 7, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 8, kImageIndexUndefined, }},
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
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 9, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 10, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 11, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 12, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 13, SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 14, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 15, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 16, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LIM_LAUNCHED_TRACK_TURN_BANK_TRANSITION + 17, kImageIndexUndefined, }},
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
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBlockBrakesSprites = {{
        {{ 15012, 15014, }},
        {{ 15013, 15015, }},
        {{ 15012, 15014, }},
        {{ 15013, 15015, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 3 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 3 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TileUp25Sprites = {{
        {{
            15689,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15688,
        }},
        {{
            15691,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15690,
        }},
        {{
            15693,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15692,
        }},
        {{
            15687,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15686,
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
            15678,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15679,
        }},
        {{
            15680,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15681,
        }},
        {{
            15682,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15683,
        }},
        {{
            15684,
            kImageIndexUndefined,
            kImageIndexUndefined,
            15685,
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
            15658,
            kImageIndexUndefined,
            15659,
            15660,
            kImageIndexUndefined,
            15661,
            15662,
        }},
        {{
            15663,
            kImageIndexUndefined,
            15664,
            15665,
            kImageIndexUndefined,
            15666,
            15667,
        }},
        {{
            15668,
            kImageIndexUndefined,
            15669,
            15670,
            kImageIndexUndefined,
            15671,
            15672,
        }},
        {{
            15673,
            kImageIndexUndefined,
            15674,
            15675,
            kImageIndexUndefined,
            15676,
            15677,
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
            15638,
            kImageIndexUndefined,
            15639,
            15640,
            kImageIndexUndefined,
            15641,
            15642,
        }},
        {{
            15643,
            kImageIndexUndefined,
            15644,
            15645,
            kImageIndexUndefined,
            15646,
            15647,
        }},
        {{
            15648,
            kImageIndexUndefined,
            15649,
            15650,
            kImageIndexUndefined,
            15651,
            15652,
        }},
        {{
            15653,
            kImageIndexUndefined,
            15654,
            15655,
            kImageIndexUndefined,
            15656,
            15657,
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 7>, kNumOrthogonalDirections>
    kRightBankedQuarterTurn5TileUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedQuarterTurn5TileUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankedUp25Sprites = {{
        {{ 15602, kImageIndexUndefined, }},
        {{ 15603, 15610, }},
        {{ 15604, kImageIndexUndefined, }},
        {{ 15605, kImageIndexUndefined, }},
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
        {{ 15606, kImageIndexUndefined, }},
        {{ 15607, kImageIndexUndefined, }},
        {{ 15608, 15611, }},
        {{ 15609, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToUp25Sprites = {{
        {{ 15612, kImageIndexUndefined, }},
        {{ 15613, 15620, }},
        {{ 15614, kImageIndexUndefined, }},
        {{ 15615, kImageIndexUndefined, }},
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
        {{ 15616, kImageIndexUndefined, }},
        {{ 15617, kImageIndexUndefined, }},
        {{ 15618, 15621, }},
        {{ 15619, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedFlatToLeftBankedUp25Sprites = {{
        15622,
        15623,
        15624,
        15625,
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
        15626,
        15627,
        15628,
        15629,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kLeftBankedUp25ToLeftBankedFlatSprites = {{
        15630,
        15631,
        15632,
        15633,
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
        15634,
        15635,
        15636,
        15637,
    }};
    
    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kRightBankedUp25ToRightBankedFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToLeftBankedFlatBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankedUp25Sprites = {{
        {{ 15574, kImageIndexUndefined, }},
        {{ 15575, 15582, }},
        {{ 15576, kImageIndexUndefined, }},
        {{ 15577, kImageIndexUndefined, }},
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
        {{ 15578, kImageIndexUndefined, }},
        {{ 15579, kImageIndexUndefined, }},
        {{ 15580, 15583, }},
        {{ 15581, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankedUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankedUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankedUp25ToFlatSprites = {{
        {{ 15584, kImageIndexUndefined, }},
        {{ 15585, 15592, }},
        {{ 15586, kImageIndexUndefined, }},
        {{ 15587, kImageIndexUndefined, }},
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
        {{ 15588, kImageIndexUndefined, }},
        {{ 15589, kImageIndexUndefined, }},
        {{ 15590, 15593, }},
        {{ 15591, kImageIndexUndefined, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankedUp25ToFlatBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankedUp25ToFlatBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagBrakesSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 0, SPR_G2_LOOPING_DIAG_BRAKES + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 2, SPR_G2_LOOPING_DIAG_BRAKES + 3, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 0, SPR_G2_LOOPING_DIAG_BRAKES + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 2, SPR_G2_LOOPING_DIAG_BRAKES + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagBlockBrakesSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 0, SPR_G2_LOOPING_DIAG_BRAKES + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 2, SPR_G2_LOOPING_DIAG_BRAKES + 3, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 0, SPR_G2_LOOPING_DIAG_BRAKES + 1, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_G2_LOOPING_DIAG_BRAKES + 2, SPR_G2_LOOPING_DIAG_BRAKES + 3, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
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
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0], nullptr, &kFlatBoundBoxes[0], false }, // Flat
        { &kBoosterSprites[0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
        { &kBoosterSprites[0], nullptr, &kStationBoundBoxes[0], false }, // BeginStation
        { &kBoosterSprites[0], nullptr, &kStationBoundBoxes[0], false }, // MiddleStation
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
        { &kLeftVerticalLoopSprites[0][0][0], &kLeftVerticalLoopOffsets[0][0][0], &kLeftVerticalLoopBoundBoxes[0][0][0], false }, // LeftVerticalLoop
        { &kRightVerticalLoopSprites[0][0][0], &kRightVerticalLoopOffsets[0][0][0], &kRightVerticalLoopBoundBoxes[0][0][0], false }, // RightVerticalLoop
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
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftTwistDownToUpSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftTwistDownToUpBoundBoxes[0][0][0], false }, // LeftTwistDownToUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightTwistDownToUpSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightTwistDownToUpBoundBoxes[0][0][0], false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true }, // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true }, // RightTwistUpToDown
        { &OpenRCT2::LimLaunchedRollerCoaster::kHalfLoopUpSprites[0][0], &OpenRCT2::LimLaunchedRollerCoaster::kHalfLoopUpOffsets[0][0], &OpenRCT2::LimLaunchedRollerCoaster::kHalfLoopUpBoundBoxes[0][0], false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true }, // HalfLoopDown
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftCorkscrewUpBoundBoxes[0][0], false }, // LeftCorkscrewUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightCorkscrewUpBoundBoxes[0][0], false }, // RightCorkscrewUp
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
        { &kFlatToUp60LongBaseSprites[0][0], nullptr, &kFlatToUp60LongBaseBoundBoxes[0][0], false }, // FlatToUp60LongBase
        { &kUp60ToFlatLongBaseSprites[0][0], nullptr, &kUp60ToFlatLongBaseBoundBoxes[0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false }, // Whirlpool
        { nullptr, nullptr, nullptr, true }, // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true }, // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false }, // CableLiftHill
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false }, // ReverseFreefallVertical
        { &OpenRCT2::LimLaunchedRollerCoaster::kUp90Sprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kUp90BoundBoxes[0][0], false }, // Up90
        { nullptr, nullptr, nullptr, true }, // Down90
        { &OpenRCT2::LimLaunchedRollerCoaster::kUp60ToUp90Sprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kUp60ToUp90BoundBoxes[0][0], false }, // Up60ToUp90
        { &OpenRCT2::LimLaunchedRollerCoaster::kDown90ToDown60Sprites[0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kDown90ToDown60BoundBoxes[0], false }, // Down90ToDown60
        { &OpenRCT2::LimLaunchedRollerCoaster::kUp90ToUp60Sprites[0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kUp90ToUp60BoundBoxes[0], false }, // Up90ToUp60
        { &OpenRCT2::LimLaunchedRollerCoaster::kDown60ToDown90Sprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kDown60ToDown90BoundBoxes[0][0], false }, // Down60ToDown90
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
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftBarrelRollUpToDownSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftBarrelRollUpToDownBoundBoxes[0][0][0], false }, // LeftBarrelRollUpToDown
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightBarrelRollUpToDownSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightBarrelRollUpToDownBoundBoxes[0][0][0], false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true }, // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true }, // RightBarrelRollDownToUp
        { &kLeftBankToLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25Offsets[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { &kRightBankToRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25Offsets[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false }, // PoweredLift
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftLargeHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftLargeHalfLoopUpBoundBoxes[0][0], false }, // LeftLargeHalfLoopUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightLargeHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightLargeHalfLoopUpBoundBoxes[0][0], false }, // RightLargeHalfLoopUp
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
        { &OpenRCT2::LimLaunchedRollerCoaster::kUp90ToInvertedFlatQuarterLoopSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kUp90ToInvertedFlatQuarterLoopBoundBoxes[0][0], false }, // Up90ToInvertedFlatQuarterLoop
        { &OpenRCT2::LimLaunchedRollerCoaster::kInvertedFlatToDown90QuarterLoopSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kInvertedFlatToDown90QuarterLoopBoundBoxes[0][0], false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true }, // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true }, // AirThrustVerticalDownToLevel
        { &kBlockBrakesSprites[0][0], nullptr, &kBlockBrakesBoundBoxes[0][0], false }, // BlockBrakes
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
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // LeftQuarterTurn1TileUp90
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightQuarterTurn1TileUp90Sprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightQuarterTurn1TileUp90BoundBoxes[0][0][0], false }, // RightQuarterTurn1TileUp90
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
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftLargeCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftLargeCorkscrewUpBoundBoxes[0][0], false }, // LeftLargeCorkscrewUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightLargeCorkscrewUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightLargeCorkscrewUpBoundBoxes[0][0], false }, // RightLargeCorkscrewUp
        { nullptr, nullptr, nullptr, true }, // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true }, // RightLargeCorkscrewDown
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftMediumHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftMediumHalfLoopUpBoundBoxes[0][0], false }, // LeftMediumHalfLoopUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightMediumHalfLoopUpSprites[0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightMediumHalfLoopUpBoundBoxes[0][0], false }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true }, // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true }, // RightMediumHalfLoopDown
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftZeroGRollUpBoundBoxes[0][0][0], false }, // LeftZeroGRollUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightZeroGRollUpBoundBoxes[0][0][0], false }, // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true }, // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true }, // RightZeroGRollDown
        { &OpenRCT2::LimLaunchedRollerCoaster::kLeftLargeZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kLeftLargeZeroGRollUpBoundBoxes[0][0][0], false }, // LeftLargeZeroGRollUp
        { &OpenRCT2::LimLaunchedRollerCoaster::kRightLargeZeroGRollUpSprites[0][0][0], nullptr, &OpenRCT2::LimLaunchedRollerCoaster::kRightLargeZeroGRollUpBoundBoxes[0][0][0], false }, // RightLargeZeroGRollUp
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
        { &kDiagBrakesSprites[0][0][0], &kDiagBrakesOffsets[0][0][0], &kDiagBrakesBoundBoxes[0][0][0], false }, // DiagBrakes
        { &kDiagBlockBrakesSprites[0][0][0], &kDiagBlockBrakesOffsets[0][0][0], &kDiagBlockBrakesBoundBoxes[0][0][0], false }, // DiagBlockBrakes
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
} // namespace OpenRCT2::LoopingRollerCoaster
