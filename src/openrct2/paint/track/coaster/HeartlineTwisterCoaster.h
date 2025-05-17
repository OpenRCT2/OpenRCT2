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

namespace OpenRCT2::HeartlineTwisterCoaster
{
    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            {{ 21294, 21296, }},
            {{ 21295, 21297, }},
            {{ 21294, 21296, }},
            {{ 21295, 21297, }},
        }},
        {{
            {{ 21354, 21356, }},
            {{ 21355, 21357, }},
            {{ 21358, 21360, }},
            {{ 21359, 21361, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            {{ 19732, kImageIndexUndefined, }},
            {{ 19733, kImageIndexUndefined, }},
            {{ 19732, kImageIndexUndefined, }},
            {{ 19733, kImageIndexUndefined, }},
        }},
        {{
            {{ 19732, 19734, }},
            {{ 19733, 19735, }},
            {{ 19732, 19734, }},
            {{ 19733, 19735, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kStationBoundBoxes = {{
        {{ { { 0, 6, 1 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
        {{ { { 6, 0, 1 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
        {{ { { 0, 6, 1 }, { 32, 20, 1 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
        {{ { { 6, 0, 1 }, { 20, 32, 1 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            {{ 21322, 21326, }},
            {{ 21323, 21327, }},
            {{ 21324, 21328, }},
            {{ 21325, 21329, }},
        }},
        {{
            {{ 21378, 21382, }},
            {{ 21379, 21383, }},
            {{ 21380, 21384, }},
            {{ 21381, 21385, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 40 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 40 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 40 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 40 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60Sprites = {{
        {{
            {{ 21346, 21350, }},
            {{ 21347, 21351, }},
            {{ 21348, 21352, }},
            {{ 21349, 21353, }},
        }},
        {{
            {{ 21402, 21406, }},
            {{ 21403, 21407, }},
            {{ 21404, 21408, }},
            {{ 21405, 21409, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 88 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 88 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 88 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 88 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            {{ 21306, 21310, }},
            {{ 21307, 21311, }},
            {{ 21308, 21312, }},
            {{ 21309, 21313, }},
        }},
        {{
            {{ 21362, 21366, }},
            {{ 21363, 21367, }},
            {{ 21364, 21368, }},
            {{ 21365, 21369, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 32 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 32 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 32 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 32 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToUp60Sprites = {{
        {{
            {{ 21330, 21334, }},
            {{ 21331, 21335, }},
            {{ 21332, 21336, }},
            {{ 21333, 21337, }},
        }},
        {{
            {{ 21386, 21390, }},
            {{ 21387, 21391, }},
            {{ 21388, 21392, }},
            {{ 21389, 21393, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 56 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 56 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 56 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 56 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp60ToUp25Sprites = {{
        {{
            {{ 21338, 21342, }},
            {{ 21339, 21343, }},
            {{ 21340, 21344, }},
            {{ 21341, 21345, }},
        }},
        {{
            {{ 21394, 21398, }},
            {{ 21395, 21399, }},
            {{ 21396, 21400, }},
            {{ 21397, 21401, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 56 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 56 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 56 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 56 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            {{ 21314, 21318, }},
            {{ 21315, 21319, }},
            {{ 21316, 21320, }},
            {{ 21317, 21321, }},
        }},
        {{
            {{ 21370, 21374, }},
            {{ 21371, 21375, }},
            {{ 21372, 21376, }},
            {{ 21373, 21377, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 32 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 32 } } }},
        {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 32 } } }},
        {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 32 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kHeartLineTransferUpSprites = {{
        {{
            {{ 21306, kImageIndexUndefined, 21310, kImageIndexUndefined, }},
            {{ 21314, 21308, 21318, 21312, }},
            {{ 21298, kImageIndexUndefined, 21302, kImageIndexUndefined, }},
            {{ 21316, kImageIndexUndefined, 21320, kImageIndexUndefined, }},
        }},
        {{
            {{ 21307, kImageIndexUndefined, 21311, kImageIndexUndefined, }},
            {{ 21315, 21309, 21319, 21313, }},
            {{ 21299, kImageIndexUndefined, 21303, kImageIndexUndefined, }},
            {{ 21317, kImageIndexUndefined, 21321, kImageIndexUndefined, }},
        }},
        {{
            {{ 21308, kImageIndexUndefined, 21312, kImageIndexUndefined, }},
            {{ 21316, 21306, 21320, 21310, }},
            {{ 21300, kImageIndexUndefined, 21304, kImageIndexUndefined, }},
            {{ 21314, kImageIndexUndefined, 21318, kImageIndexUndefined, }},
        }},
        {{
            {{ 21309, kImageIndexUndefined, 21313, kImageIndexUndefined, }},
            {{ 21317, 21307, 21321, 21311, }},
            {{ 21301, kImageIndexUndefined, 21305, kImageIndexUndefined, }},
            {{ 21315, kImageIndexUndefined, 21319, kImageIndexUndefined, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
    kHeartLineTransferUpOffsets = {{
        {{
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, 8 }, { 0, 0, 16 }, { 0, 0, 8 }, { 0, 0, 16 } }},
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, -8 }, {}, { 0, 0, -8 }, {} }},
        }},
        {{
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, 8 }, { 0, 0, 16 }, { 0, 0, 8 }, { 0, 0, 16 } }},
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, -8 }, {}, { 0, 0, -8 }, {} }},
        }},
        {{
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, 8 }, { 0, 0, 16 }, { 0, 0, 8 }, { 0, 0, 16 } }},
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, -8 }, {}, { 0, 0, -8 }, {} }},
        }},
        {{
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, 8 }, { 0, 0, 16 }, { 0, 0, 8 }, { 0, 0, 16 } }},
            {{ { 0, 0, 0 }, {}, { 0, 0, 0 }, {} }},
            {{ { 0, 0, -8 }, {}, { 0, 0, -8 }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
    kHeartLineTransferUpBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, {}, { { 0, 6, 23 }, { 32, 20, 0 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 2 } }, { { 0, 6, 16 }, { 32, 20, 2 } }, { { 0, 27, 8 }, { 32, 1, 34 } }, { { 0, 27, 16 }, { 32, 1, 34 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, {}, { { 0, 27, 0 }, { 32, 1, 24 } }, {} }},
            {{ { { 0, 6, -7 }, { 32, 20, 2 } }, {}, { { 0, 27, -8 }, { 32, 1, 32 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, {}, { { 6, 0, 23 }, { 20, 32, 0 } }, {} }},
            {{ { { 6, 0, 8 }, { 20, 32, 2 } }, { { 6, 0, 16 }, { 20, 32, 2 } }, { { 27, 0, 8 }, { 1, 32, 34 } }, { { 27, 0, 16 }, { 1, 32, 34 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, {}, { { 27, 0, 0 }, { 1, 32, 24 } }, {} }},
            {{ { { 6, 0, -7 }, { 20, 32, 2 } }, {}, { { 27, 0, -8 }, { 1, 32, 32 } }, {} }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, {}, { { 0, 6, 23 }, { 32, 20, 0 } }, {} }},
            {{ { { 0, 6, 8 }, { 32, 20, 2 } }, { { 0, 6, 16 }, { 32, 20, 2 } }, { { 0, 27, 8 }, { 32, 1, 34 } }, { { 0, 27, 16 }, { 32, 1, 34 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, {}, { { 0, 27, 0 }, { 32, 1, 24 } }, {} }},
            {{ { { 0, 6, -7 }, { 32, 20, 2 } }, {}, { { 0, 27, -8 }, { 32, 1, 32 } }, {} }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, {}, { { 6, 0, 23 }, { 20, 32, 0 } }, {} }},
            {{ { { 6, 0, 8 }, { 20, 32, 2 } }, { { 6, 0, 16 }, { 20, 32, 2 } }, { { 27, 0, 8 }, { 1, 32, 34 } }, { { 27, 0, 16 }, { 1, 32, 34 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, {}, { { 27, 0, 0 }, { 1, 32, 24 } }, {} }},
            {{ { { 6, 0, -7 }, { 20, 32, 2 } }, {}, { { 27, 0, -8 }, { 1, 32, 32 } }, {} }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 6>, kNumOrthogonalDirections>
    kLeftHeartLineRollSprites = {{
        {{
            {{ 21410, 21422, }},
            {{ 21411, 21423, }},
            {{ 21412, 21424, }},
            {{ 21418, 21430, }},
            {{ 21417, 21429, }},
            {{ 21416, 21428, }},
        }},
        {{
            {{ 21413, 21425, }},
            {{ 21414, 21426, }},
            {{ 21415, 21427, }},
            {{ 21421, 21433, }},
            {{ 21420, 21432, }},
            {{ 21419, 21431, }},
        }},
        {{
            {{ 21416, 21428, }},
            {{ 21417, 21429, }},
            {{ 21418, 21430, }},
            {{ 21412, 21424, }},
            {{ 21411, 21423, }},
            {{ 21410, 21422, }},
        }},
        {{
            {{ 21419, 21431, }},
            {{ 21420, 21432, }},
            {{ 21421, 21433, }},
            {{ 21415, 21427, }},
            {{ 21414, 21426, }},
            {{ 21413, 21425, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 6>, kNumOrthogonalDirections>
    kLeftHeartLineRollBoundBoxes = {{
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
        }},
        {{
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
            {{ { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 27, 0 }, { 32, 1, 24 } } }},
        }},
        {{
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
            {{ { { 6, 0, 0 }, { 20, 32, 2 } }, { { 27, 0, 0 }, { 1, 32, 24 } } }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 6>, kNumOrthogonalDirections>
    kRightHeartLineRollSprites = {{
        {{
            {{ 21434, 21446, }},
            {{ 21435, 21447, }},
            {{ 21436, 21448, }},
            {{ 21442, 21454, }},
            {{ 21441, 21453, }},
            {{ 21440, 21452, }},
        }},
        {{
            {{ 21437, 21449, }},
            {{ 21438, 21450, }},
            {{ 21439, 21451, }},
            {{ 21445, 21457, }},
            {{ 21444, 21456, }},
            {{ 21443, 21455, }},
        }},
        {{
            {{ 21440, 21452, }},
            {{ 21441, 21453, }},
            {{ 21442, 21454, }},
            {{ 21436, 21448, }},
            {{ 21435, 21447, }},
            {{ 21434, 21446, }},
        }},
        {{
            {{ 21443, 21455, }},
            {{ 21444, 21456, }},
            {{ 21445, 21457, }},
            {{ 21439, 21451, }},
            {{ 21438, 21450, }},
            {{ 21437, 21449, }},
        }},
    }};
    
    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 6>, kNumOrthogonalDirections>
    kRightHeartLineRollBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHeartLineRollBoundBoxes);
    
    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // Flat
        { &kStationSprites[0][0][0], nullptr, &kStationBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], nullptr, &kStationBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], nullptr, &kStationBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0][0][0], nullptr, &kUp25BoundBoxes[0][0], false }, // Up25
        { &kUp60Sprites[0][0][0], nullptr, &kUp60BoundBoxes[0][0], false }, // Up60
        { &kFlatToUp25Sprites[0][0][0], nullptr, &kFlatToUp25BoundBoxes[0][0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0][0], nullptr, &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0][0], nullptr, &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0][0], nullptr, &kUp25ToFlatBoundBoxes[0][0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
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
        { nullptr, nullptr, nullptr, false }, // SBendLeft
        { nullptr, nullptr, nullptr, false }, // SBendRight
        { nullptr, nullptr, nullptr, false }, // LeftVerticalLoop
        { nullptr, nullptr, nullptr, false }, // RightVerticalLoop
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn3Tiles
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
        { &kHeartLineTransferUpSprites[0][0][0], &kHeartLineTransferUpOffsets[0][0][0], &kHeartLineTransferUpBoundBoxes[0][0][0], false }, // HeartLineTransferUp
        { nullptr, nullptr, nullptr, true }, // HeartLineTransferDown
        { &kLeftHeartLineRollSprites[0][0][0], nullptr, &kLeftHeartLineRollBoundBoxes[0][0][0], false }, // LeftHeartLineRoll
        { &kRightHeartLineRollSprites[0][0][0], nullptr, &kRightHeartLineRollBoundBoxes[0][0][0], false }, // RightHeartLineRoll
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
} // namespace OpenRCT2::HeartlineTwisterCoaster
