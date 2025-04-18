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

namespace OpenRCT2::BobsleighCoaster
{
    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            {{ 14572, 14574, }},
            {{ 14573, 14575, }},
            {{ 14572, 14574, }},
            {{ 14573, 14575, }},
        }},
        {{
            {{ 14576, 14578, }},
            {{ 14577, 14579, }},
            {{ 14576, 14578, }},
            {{ 14577, 14579, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};

    static constexpr std::array<ImageIndex, kNumOrthogonalDirections>
    kStationSprites = {{
        14580,
        14581,
        14580,
        14581,
    }};

    static constexpr std::array<BoundBoxXYZ, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        { { 0, 6, 1 }, { 32, 20, 1 } },
        { { 6, 0, 1 }, { 20, 32, 1 } },
        { { 0, 6, 1 }, { 32, 20, 1 } },
        { { 6, 0, 1 }, { 20, 32, 1 } },
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            {{ 14610, 14614, }},
            {{ 14611, 14615, }},
            {{ 14612, 14616, }},
            {{ 14613, 14617, }},
        }},
        {{
            {{ 14634, 14638, }},
            {{ 14635, 14639, }},
            {{ 14636, 14640, }},
            {{ 14637, 14641, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 50 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 50 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            {{ 14594, 14598, }},
            {{ 14595, 14599, }},
            {{ 14596, 14600, }},
            {{ 14597, 14601, }},
        }},
        {{
            {{ 14618, 14622, }},
            {{ 14619, 14623, }},
            {{ 14620, 14624, }},
            {{ 14621, 14625, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 42 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 42 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            {{ 14602, 14606, }},
            {{ 14603, 14607, }},
            {{ 14604, 14608, }},
            {{ 14605, 14609, }},
        }},
        {{
            {{ 14626, 14630, }},
            {{ 14627, 14631, }},
            {{ 14628, 14632, }},
            {{ 14629, 14633, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 34 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 34 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 14707, 14727, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14706, 14726, }},
            {{ 14705, 14725, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14704, 14724, }},
            {{ 14703, 14723, }},
        }},
        {{
            {{ 14712, 14732, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14711, 14731, }},
            {{ 14710, 14730, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14709, 14729, }},
            {{ 14708, 14728, }},
        }},
        {{
            {{ 14717, 14737, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14716, 14736, }},
            {{ 14715, 14735, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14714, 14734, }},
            {{ 14713, 14733, }},
        }},
        {{
            {{ 14702, 14722, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14701, 14721, }},
            {{ 14700, 14720, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14699, 14719, }},
            {{ 14698, 14718, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesOffsets = {{
        {{
            {{ { 0, 2, 0 }, { 0, 2, 0 } }},
            {{ {}, {} }},
            {{ { 0, 0, 0 }, { 0, 0, 0 } }},
            {{ { 0, 16, 0 }, { 0, 16, 0 } }},
            {{ {}, {} }},
            {{ { 16, 0, 0 }, { 16, 0, 0 } }},
            {{ { 2, 0, 0 }, { 2, 0, 0 } }},
        }},
        {{
            {{ { 2, 0, 0 }, { 2, 0, 0 } }},
            {{ {}, {} }},
            {{ { 0, 0, 0 }, { 0, 0, 0 } }},
            {{ { 16, 16, 0 }, { 16, 16, 0 } }},
            {{ {}, {} }},
            {{ { 0, 0, 0 }, { 0, 0, 0 } }},
            {{ { 0, 2, 0 }, { 0, 2, 0 } }},
        }},
        {{
            {{ { 0, 2, 0 }, { 0, 2, 0 } }},
            {{ {}, {} }},
            {{ { 0, 16, 0 }, { 0, 16, 0 } }},
            {{ { 16, 0, 0 }, { 16, 0, 0 } }},
            {{ {}, {} }},
            {{ { 0, 0, 0 }, { 0, 0, 0 } }},
            {{ { 2, 0, 0 }, { 2, 0, 0 } }},
        }},
        {{
            {{ { 2, 0, 0 }, { 2, 0, 0 } }},
            {{ {}, {} }},
            {{ { 16, 0, 0 }, { 16, 0, 0 } }},
            {{ { 0, 0, 0 }, { 0, 0, 0 } }},
            {{ {}, {} }},
            {{ { 0, 16, 0 }, { 0, 16, 0 } }},
            {{ { 0, 2, 0 }, { 0, 2, 0 } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 34, 2 } }, { { 16, 0, 27 }, { 16, 34, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 30, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        {{ 14642, 14646, }},
        {{ 14643, 14647, }},
        {{ 14644, 14648, }},
        {{ 14645, 14649, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        {{ 14650, 14654, }},
        {{ 14651, 14655, }},
        {{ 14652, 14656, }},
        {{ 14653, 14657, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 14747, 14767, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14746, 14766, }},
            {{ 14745, 14765, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14744, 14764, }},
            {{ 14743, 14763, }},
        }},
        {{
            {{ 14752, 14772, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14751, 14771, }},
            {{ 14750, 14770, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14749, 14769, }},
            {{ 14748, 14768, }},
        }},
        {{
            {{ 14757, 14777, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14756, 14776, }},
            {{ 14755, 14775, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14754, 14774, }},
            {{ 14753, 14773, }},
        }},
        {{
            {{ 14742, 14762, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14741, 14761, }},
            {{ 14740, 14760, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14739, 14759, }},
            {{ 14738, 14758, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 7>, kNumOrthogonalDirections>
    kBankedLeftQuarterTurn5TilesBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        {{ 14674, 14678, }},
        {{ 14675, 14679, }},
        {{ 14676, 14680, }},
        {{ 14677, 14681, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftBankToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        {{ 14682, 14686, }},
        {{ 14683, 14687, }},
        {{ 14684, 14688, }},
        {{ 14685, 14689, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ 14658, 14662, }},
        {{ 14659, 14663, }},
        {{ 14660, 14664, }},
        {{ 14661, 14665, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        {{ 14666, 14670, }},
        {{ 14667, 14671, }},
        {{ 14668, 14672, }},
        {{ 14669, 14673, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        {{ 14690, 14694, }},
        {{ 14691, 14695, }},
        {{ 14692, 14696, }},
        {{ 14693, 14697, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kLeftBankBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            {{ 14826, 14842, }},
            {{ 14827, 14843, }},
            {{ 14828, 14844, }},
            {{ 14829, 14845, }},
        }},
        {{
            {{ 14830, 14846, }},
            {{ 14831, 14847, }},
            {{ 14832, 14848, }},
            {{ 14833, 14849, }},
        }},
        {{
            {{ 14829, 14845, }},
            {{ 14828, 14844, }},
            {{ 14827, 14843, }},
            {{ 14826, 14842, }},
        }},
        {{
            {{ 14833, 14849, }},
            {{ 14832, 14848, }},
            {{ 14831, 14847, }},
            {{ 14830, 14846, }},
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
            {{ 14834, 14850, }},
            {{ 14835, 14851, }},
            {{ 14836, 14852, }},
            {{ 14837, 14853, }},
        }},
        {{
            {{ 14838, 14854, }},
            {{ 14839, 14855, }},
            {{ 14840, 14856, }},
            {{ 14841, 14857, }},
        }},
        {{
            {{ 14837, 14853, }},
            {{ 14836, 14852, }},
            {{ 14835, 14851, }},
            {{ 14834, 14850, }},
        }},
        {{
            {{ 14841, 14857, }},
            {{ 14840, 14856, }},
            {{ 14839, 14855, }},
            {{ 14838, 14854, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            {{ 14783, 14795, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14782, 14794, }},
            {{ 14781, 14793, }},
        }},
        {{
            {{ 14786, 14798, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14785, 14797, }},
            {{ 14784, 14796, }},
        }},
        {{
            {{ 14789, 14801, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14788, 14800, }},
            {{ 14787, 14799, }},
        }},
        {{
            {{ 14780, 14792, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14779, 14791, }},
            {{ 14778, 14790, }},
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
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            {{ 14807, 14819, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14806, 14818, }},
            {{ 14805, 14817, }},
        }},
        {{
            {{ 14810, 14822, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14809, 14821, }},
            {{ 14808, 14820, }},
        }},
        {{
            {{ 14813, 14825, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14812, 14824, }},
            {{ 14811, 14823, }},
        }},
        {{
            {{ 14804, 14816, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14803, 14815, }},
            {{ 14802, 14814, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesBoundBoxes = {{
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
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 14887, 14899, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14886, 14898, }},
            {{ 14885, 14897, }},
            {{ 14884, 14896, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14883, 14895, }},
            {{ 14882, 14894, }},
        }},
        {{
            {{ 14890, 14902, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14889, 14901, }},
            {{ 14888, 14900, }},
            {{ 14887, 14899, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14886, 14898, }},
            {{ 14885, 14897, }},
        }},
        {{
            {{ 14893, 14905, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14892, 14904, }},
            {{ 14891, 14903, }},
            {{ 14890, 14902, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14889, 14901, }},
            {{ 14888, 14900, }},
        }},
        {{
            {{ 14884, 14896, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14883, 14895, }},
            {{ 14882, 14894, }},
            {{ 14893, 14905, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14892, 14904, }},
            {{ 14891, 14903, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallSprites = {{
        {{
            {{ 14858, 14870, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14859, 14871, }},
            {{ 14860, 14872, }},
            {{ 14861, 14873, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14862, 14874, }},
            {{ 14863, 14875, }},
        }},
        {{
            {{ 14861, 14873, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14862, 14874, }},
            {{ 14863, 14875, }},
            {{ 14864, 14876, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14865, 14877, }},
            {{ 14866, 14878, }},
        }},
        {{
            {{ 14864, 14876, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14865, 14877, }},
            {{ 14866, 14878, }},
            {{ 14867, 14879, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14868, 14880, }},
            {{ 14869, 14881, }},
        }},
        {{
            {{ 14867, 14879, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14868, 14880, }},
            {{ 14869, 14881, }},
            {{ 14858, 14870, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14859, 14871, }},
            {{ 14860, 14872, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 14955, 14975, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14954, 14974, }},
            {{ 14953, 14973, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14952, 14972, }},
            {{ 14951, 14971, }},
            {{ 14950, 14970, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14949, 14969, }},
            {{ 14948, 14968, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14947, 14967, }},
            {{ 14946, 14966, }},
        }},
        {{
            {{ 14960, 14980, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14959, 14979, }},
            {{ 14958, 14978, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14957, 14977, }},
            {{ 14956, 14976, }},
            {{ 14955, 14975, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14954, 14974, }},
            {{ 14953, 14973, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14952, 14972, }},
            {{ 14951, 14971, }},
        }},
        {{
            {{ 14965, 14985, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14964, 14984, }},
            {{ 14963, 14983, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14962, 14982, }},
            {{ 14961, 14981, }},
            {{ 14960, 14980, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14959, 14979, }},
            {{ 14958, 14978, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14957, 14977, }},
            {{ 14956, 14976, }},
        }},
        {{
            {{ 14950, 14970, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14949, 14969, }},
            {{ 14948, 14968, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14947, 14967, }},
            {{ 14946, 14966, }},
            {{ 14965, 14985, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14964, 14984, }},
            {{ 14963, 14983, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14962, 14982, }},
            {{ 14961, 14981, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 8 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 6, 8 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 27 }, { 32, 20, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 27 }, { 20, 32, 0 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeSprites = {{
        {{
            {{ 14906, 14926, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14907, 14927, }},
            {{ 14908, 14928, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14909, 14929, }},
            {{ 14910, 14930, }},
            {{ 14911, 14931, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14912, 14932, }},
            {{ 14913, 14933, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14914, 14934, }},
            {{ 14915, 14935, }},
        }},
        {{
            {{ 14911, 14931, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14912, 14932, }},
            {{ 14913, 14933, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14914, 14934, }},
            {{ 14915, 14935, }},
            {{ 14916, 14936, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14917, 14937, }},
            {{ 14918, 14938, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14919, 14939, }},
            {{ 14920, 14940, }},
        }},
        {{
            {{ 14916, 14936, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14917, 14937, }},
            {{ 14918, 14938, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14919, 14939, }},
            {{ 14920, 14940, }},
            {{ 14921, 14941, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14922, 14942, }},
            {{ 14923, 14943, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14924, 14944, }},
            {{ 14925, 14945, }},
        }},
        {{
            {{ 14921, 14941, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14922, 14942, }},
            {{ 14923, 14943, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14924, 14944, }},
            {{ 14925, 14945, }},
            {{ 14906, 14926, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14907, 14927, }},
            {{ 14908, 14928, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 14909, 14929, }},
            {{ 14910, 14930, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);
    
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBrakesSprites = {{
        {{ 14582, 14584, }},
        {{ 14583, 14585, }},
        {{ 14582, 14584, }},
        {{ 14583, 14585, }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kBlockBrakesSprites = {{
        {{
            {{ 14586, 14590, }},
            {{ 14587, 14591, }},
            {{ 14586, 14590, }},
            {{ 14587, 14591, }},
        }},
        {{
            {{ 14588, 14590, }},
            {{ 14589, 14591, }},
            {{ 14588, 14590, }},
            {{ 14589, 14591, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBlockBrakesBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 26 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 26 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kOnRidePhotoSprites = {{
        {{ 14572, 14574, }},
        {{ 14573, 14575, }},
        {{ 14572, 14574, }},
        {{ 14573, 14575, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kOnRidePhotoBoundBoxes = {{
        {{ { { 0, 6, 1 }, { 32, 20, 1 } }, { { 0, 27, 1 }, { 32, 1, 22 } } }},
        {{ { { 6, 0, 1 }, { 20, 32, 1 } }, { { 27, 0, 1 }, { 1, 32, 22 } } }},
        {{ { { 0, 6, 1 }, { 32, 20, 1 } }, { { 0, 27, 1 }, { 32, 1, 22 } } }},
        {{ { { 6, 0, 1 }, { 20, 32, 1 } }, { { 27, 0, 1 }, { 1, 32, 22 } } }},
    }};
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // Flat
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false }, // EndStation
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false }, // BeginStation
        { &kStationSprites[0], nullptr, &kStationBoundBoxes[0], false }, // MiddleStation
        { &kUp25Sprites[0][0][0], nullptr, &kUp25BoundBoxes[0][0], false }, // Up25
        { nullptr, nullptr, nullptr, false }, // Up60
        { &kFlatToUp25Sprites[0][0][0], nullptr, &kFlatToUp25BoundBoxes[0][0], false }, // FlatToUp25
        { nullptr, nullptr, nullptr, false }, // Up25ToUp60
        { nullptr, nullptr, nullptr, false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0][0], nullptr, &kUp25ToFlatBoundBoxes[0][0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0][0], &kLeftQuarterTurn5TilesOffsets[0][0][0], &kLeftQuarterTurn5TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn5Tiles
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
        { &kLeftBankSprites[0][0], nullptr, &kLeftBankBoundBoxes[0][0], false }, // LeftBank
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
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftBankedQuarterTurn3Tiles
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
        { &kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0], false }, // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn1TileDown60
        { &kBrakesSprites[0][0], nullptr, &kBrakesBoundBoxes[0][0], false }, // Brakes
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
        { &kOnRidePhotoSprites[0][0], nullptr, &kOnRidePhotoBoundBoxes[0][0], false }, // OnRidePhoto
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
        { &kBlockBrakesSprites[0][0][0], nullptr, &kBlockBrakesBoundBoxes[0][0], false }, // BlockBrakes
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
} // namespace OpenRCT2::BobsleighCoaster
