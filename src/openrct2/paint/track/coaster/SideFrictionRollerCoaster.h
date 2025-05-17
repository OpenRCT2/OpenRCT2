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

namespace OpenRCT2::SideFrictionRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatSprites = {{
        {{
            {{ 21606, 21608, }},
            {{ 21607, 21609, }},
            {{ 21606, 21608, }},
            {{ 21607, 21609, }},
        }},
        {{
            {{ 21662, 21664, }},
            {{ 21663, 21665, }},
            {{ 21666, 21668, }},
            {{ 21667, 21669, }},
        }},
    }};
    
    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
    }};
    
    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kStationSprites = {{
        {{
            {{ 21610, kImageIndexUndefined, }},
            {{ 21611, kImageIndexUndefined, }},
            {{ 21610, kImageIndexUndefined, }},
            {{ 21611, kImageIndexUndefined, }},
        }},
        {{
            {{ 21610, 21612, }},
            {{ 21611, 21613, }},
            {{ 21610, 21612, }},
            {{ 21611, 21613, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25Sprites = {{
        {{
            {{ 21622, 21634, }},
            {{ 21623, 21635, }},
            {{ 21624, 21636, }},
            {{ 21625, 21637, }},
        }},
        {{
            {{ 21678, 21690, }},
            {{ 21679, 21691, }},
            {{ 21680, 21692, }},
            {{ 21681, 21693, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 20 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 20 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60Sprites = {{
        {{ 21646, 21658, }},
        {{ 21647, 21659, }},
        {{ 21648, 21660, }},
        {{ 21649, 21661, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kFlatToUp25Sprites = {{
        {{
            {{ 21614, 21626, }},
            {{ 21615, 21627, }},
            {{ 21616, 21628, }},
            {{ 21617, 21629, }},
        }},
        {{
            {{ 21670, 21682, }},
            {{ 21671, 21683, }},
            {{ 21672, 21684, }},
            {{ 21673, 21685, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kFlatToUp25BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 16 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 16 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp25ToUp60Sprites = {{
        {{ 21638, 21650, }},
        {{ 21639, 21651, }},
        {{ 21640, 21652, }},
        {{ 21641, 21653, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToUp60BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 40 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 40 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
    }};

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kUp60ToUp25Sprites = {{
        {{ 21642, 21654, }},
        {{ 21643, 21655, }},
        {{ 21644, 21656, }},
        {{ 21645, 21657, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp60ToUp25BoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 36 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 36 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>, 2>
    kUp25ToFlatSprites = {{
        {{
            {{ 21618, 21630, }},
            {{ 21619, 21631, }},
            {{ 21620, 21632, }},
            {{ 21621, 21633, }},
        }},
        {{
            {{ 21674, 21686, }},
            {{ 21675, 21687, }},
            {{ 21676, 21688, }},
            {{ 21677, 21689, }},
        }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kUp25ToFlatBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 9 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 16 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 26, 5 }, { 32, 1, 16 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 26, 0, 5 }, { 1, 32, 9 } } }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 7>, kNumOrthogonalDirections>
    kLeftQuarterTurn5TilesSprites = {{
        {{
            {{ 21727, 21747, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21726, 21746, }},
            {{ 21725, 21745, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21724, 21744, }},
            {{ 21723, 21743, }},
        }},
        {{
            {{ 21732, 21752, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21731, 21751, }},
            {{ 21730, 21750, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21729, 21749, }},
            {{ 21728, 21748, }},
        }},
        {{
            {{ 21737, 21757, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21736, 21756, }},
            {{ 21735, 21755, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21734, 21754, }},
            {{ 21733, 21753, }},
        }},
        {{
            {{ 21722, 21742, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21721, 21741, }},
            {{ 21720, 21740, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21719, 21739, }},
            {{ 21718, 21738, }},
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
            {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 34, 2 } }, { { 16, 0, 27 }, { 16, 34, 0 } } }},
            {{ { { 2, 0, 0 }, { 32, 32, 2 } }, { { 2, 0, 27 }, { 32, 32, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        }},
        {{
            {{ { { 0, 2, 0 }, { 32, 32, 2 } }, { { 0, 2, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
        }},
        {{
            {{ { { 2, 0, 0 }, { 32, 32, 2 } }, { { 2, 0, 27 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 2, 0 }, { 32, 32, 2 } }, { { 0, 2, 27 }, { 32, 32, 0 } } }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kSBendLeftSprites = {{
        {{
            {{ 21888, 21904, }},
            {{ 21889, 21905, }},
            {{ 21890, 21906, }},
            {{ 21891, 21907, }},
        }},
        {{
            {{ 21892, 21908, }},
            {{ 21893, 21909, }},
            {{ 21894, 21910, }},
            {{ 21895, 21911, }},
        }},
        {{
            {{ 21891, 21907, }},
            {{ 21890, 21906, }},
            {{ 21889, 21905, }},
            {{ 21888, 21904, }},
        }},
        {{
            {{ 21895, 21911, }},
            {{ 21894, 21910, }},
            {{ 21893, 21909, }},
            {{ 21892, 21908, }},
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
            {{ 21896, 21912, }},
            {{ 21897, 21913, }},
            {{ 21898, 21914, }},
            {{ 21899, 21915, }},
        }},
        {{
            {{ 21900, 21916, }},
            {{ 21901, 21917, }},
            {{ 21902, 21918, }},
            {{ 21903, 21919, }},
        }},
        {{
            {{ 21899, 21915, }},
            {{ 21898, 21914, }},
            {{ 21897, 21913, }},
            {{ 21896, 21912, }},
        }},
        {{
            {{ 21903, 21919, }},
            {{ 21902, 21918, }},
            {{ 21901, 21917, }},
            {{ 21900, 21916, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kSBendRightBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kSBendLeftBoundBoxes);

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kLeftQuarterTurn3TilesSprites = {{
        {{
            {{ 21699, 21711, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21698, 21710, }},
            {{ 21697, 21709, }},
        }},
        {{
            {{ 21702, 21714, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21701, 21713, }},
            {{ 21700, 21712, }},
        }},
        {{
            {{ 21705, 21717, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21704, 21716, }},
            {{ 21703, 21715, }},
        }},
        {{
            {{ 21696, 21708, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21695, 21707, }},
            {{ 21694, 21706, }},
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

    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kBrakesSprites = {{
        {{ 21610, 21612, }},
        {{ 21611, 21613, }},
        {{ 21610, 21612, }},
        {{ 21611, 21613, }},
    }};

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections>
    kBrakesBoundBoxes = {{
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
        {{ { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 27 }, { 32, 27, 0 } } }},
        {{ { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 27 }, { 27, 32, 0 } } }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagSprites = {{
        {{
            {{ 21790, 21806, }},
            {{ 21791, 21807, }},
            {{ 21792, 21808, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21793, 21809, }},
        }},
        {{
            {{ 21794, 21810, }},
            {{ 21795, 21811, }},
            {{ 21796, 21812, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21797, 21813, }},
        }},
        {{
            {{ 21798, 21814, }},
            {{ 21799, 21815, }},
            {{ 21800, 21816, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21801, 21817, }},
        }},
        {{
            {{ 21802, 21818, }},
            {{ 21803, 21819, }},
            {{ 21804, 21820, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21805, 21821, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kLeftEighthToDiagBoundBoxes = {{
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 27 }, { 32, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 27 }, { 32, 16, 0 } } }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 27 }, { 16, 16, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 27 }, { 32, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 34, 2 } }, { { 0, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 26 }, { 18, 18, 0 } } }},
            {{ {}, {} }},
            {{ { { 16, 0, 0 }, { 18, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 27 }, { 32, 32, 0 } } }},
            {{ { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 27 }, { 32, 16, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
        }},
        {{
            {{ { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 27 }, { 32, 32, 0 } } }},
            {{ { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 27 }, { 16, 32, 0 } } }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 27 }, { 16, 16, 0 } } }},
            {{ {}, {} }},
            {{ { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 27 }, { 16, 16, 0 } } }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagSprites = {{
        {{
            {{ 21758, 21774, }},
            {{ 21759, 21775, }},
            {{ 21760, 21776, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21761, 21777, }},
        }},
        {{
            {{ 21762, 21778, }},
            {{ 21763, 21779, }},
            {{ 21764, 21780, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21765, 21781, }},
        }},
        {{
            {{ 21766, 21782, }},
            {{ 21767, 21783, }},
            {{ 21768, 21784, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21769, 21785, }},
        }},
        {{
            {{ 21770, 21786, }},
            {{ 21771, 21787, }},
            {{ 21772, 21788, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21773, 21789, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 5>, kNumOrthogonalDirections>
    kRightEighthToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthToDiagBoundBoxes);

    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21822, 21826, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21823, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21824, 21827, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21825, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21846, 21850, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21847, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21848, 21851, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21849, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatOffsets = {{
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
    kDiagFlatBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 24 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 24 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25Sprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21840, 21844, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21841, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21842, 21845, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21843, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21864, 21868, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21865, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21866, 21869, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21867, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25Offsets = {{
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
    kDiagUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 43 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 43 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21882, 21886, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21883, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21884, 21887, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 21885, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60Offsets = {{
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
    kDiagUp60BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 43 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 43 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>, 2>
    kDiagFlatToUp25Sprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21828, 21832, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21829, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21830, 21833, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21831, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21852, 21856, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21853, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21854, 21857, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21855, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagFlatToUp25Offsets = {{
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
    kDiagFlatToUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21870, 24033, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21871, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21872, 24034, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 21873, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60Offsets = {{
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
            {{ { -16, -16, 0 }, { -16, -16, 0 } }},
            {{ {}, {} }},
        }},
        {{
            {{ { -14, -12, 0 }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToUp60BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 59 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 59 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21876, 21880, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21877, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ 21878, 21881, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ 21879, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp60ToUp25Offsets = {{
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
    kDiagUp60ToUp25BoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 59 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { 0, 0, 0 }, { 16, 16, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 59 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    static constexpr std::array<std::array<std::array<std::array<ImageIndex, 2>, 4>, kNumOrthogonalDirections>, 2>
    kDiagUp25ToFlatSprites = {{
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21834, 21838, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21835, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21836, 21839, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21837, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
        {{
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21858, 21862, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21859, kImageIndexUndefined, }},
            }},
            {{
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ 21860, 21863, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
            {{
                {{ 21861, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
                {{ kImageIndexUndefined, kImageIndexUndefined, }},
            }},
        }},
    }};

    static constexpr std::array<std::array<std::array<CoordsXYZ, 2>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToFlatOffsets = {{
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
    kDiagUp25ToFlatBoundBoxes = {{
        {{
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
        }},
        {{
            {{ {}, {} }},
            {{ {}, {} }},
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 35 }, { 32, 32, 0 } } }},
            {{ {}, {} }},
        }},
        {{
            {{ { { -16, -16, 0 }, { 32, 32, 2 } }, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
            {{ {}, {} }},
        }},
    }};

    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &kFlatSprites[0][0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // Flat
        { &kStationSprites[0][0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // EndStation
        { &kStationSprites[0][0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // BeginStation
        { &kStationSprites[0][0][0], nullptr, &kFlatBoundBoxes[0][0], false }, // MiddleStation
        { &kUp25Sprites[0][0][0], nullptr, &kUp25BoundBoxes[0][0], false }, // Up25
        { &kUp60Sprites[0][0], nullptr, &kUp60BoundBoxes[0][0], false }, // Up60
        { &kFlatToUp25Sprites[0][0][0], nullptr, &kFlatToUp25BoundBoxes[0][0], false }, // FlatToUp25
        { &kUp25ToUp60Sprites[0][0], nullptr, &kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &kUp60ToUp25Sprites[0][0], nullptr, &kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &kUp25ToFlatSprites[0][0][0], nullptr, &kUp25ToFlatBoundBoxes[0][0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true }, // Down25
        { nullptr, nullptr, nullptr, true }, // Down60
        { nullptr, nullptr, nullptr, true }, // FlatToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToDown60
        { nullptr, nullptr, nullptr, true }, // Down60ToDown25
        { nullptr, nullptr, nullptr, true }, // Down25ToFlat
        { &kLeftQuarterTurn5TilesSprites[0][0][0], &kLeftQuarterTurn5TilesOffsets[0][0][0], &kLeftQuarterTurn5TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn5Tiles
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
        { &kLeftEighthToDiagSprites[0][0][0], nullptr, &kLeftEighthToDiagBoundBoxes[0][0][0], false }, // LeftEighthToDiag
        { &kRightEighthToDiagSprites[0][0][0], nullptr, &kRightEighthToDiagBoundBoxes[0][0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthToOrthogonal
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiag
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &kDiagFlatSprites[0][0][0][0], &kDiagFlatOffsets[0][0][0], &kDiagFlatBoundBoxes[0][0][0], false }, // DiagFlat
        { &kDiagUp25Sprites[0][0][0][0], &kDiagUp25Offsets[0][0][0], &kDiagUp25BoundBoxes[0][0][0], false }, // DiagUp25
        { &kDiagUp60Sprites[0][0][0], &kDiagUp60Offsets[0][0][0], &kDiagUp60BoundBoxes[0][0][0], false }, // DiagUp60
        { &kDiagFlatToUp25Sprites[0][0][0][0], &kDiagFlatToUp25Offsets[0][0][0], &kDiagFlatToUp25BoundBoxes[0][0][0], false }, // DiagFlatToUp25
        { &kDiagUp25ToUp60Sprites[0][0][0], &kDiagUp25ToUp60Offsets[0][0][0], &kDiagUp25ToUp60BoundBoxes[0][0][0], false }, // DiagUp25ToUp60
        { &kDiagUp60ToUp25Sprites[0][0][0], &kDiagUp60ToUp25Offsets[0][0][0], &kDiagUp60ToUp25BoundBoxes[0][0][0], false }, // DiagUp60ToUp25
        { &kDiagUp25ToFlatSprites[0][0][0][0], &kDiagUp25ToFlatOffsets[0][0][0], &kDiagUp25ToFlatBoundBoxes[0][0][0], false }, // DiagUp25ToFlat
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
} // namespace OpenRCT2::SideFrictionRollerCoaster
