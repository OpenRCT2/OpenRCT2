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
#include "../../../paint/track/coaster/WoodenRollerCoaster.h"
#include "../../../ride/RideData.h"
#include "../../../world/Location.hpp"

#include <array>

namespace OpenRCT2::ClassicWoodenTwisterRollerCoaster
{
    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToLeftBankSprites = {{
        {{ SPR_CSG_BEGIN + 65447, SPR_CSG_BEGIN + 66203, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65448, SPR_CSG_BEGIN + 66204, SPR_CSG_BEGIN + 65475, SPR_CSG_BEGIN + 66231, }},
        {{ SPR_CSG_BEGIN + 65449, SPR_CSG_BEGIN + 66205, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65450, SPR_CSG_BEGIN + 66206, SPR_CSG_BEGIN + 65476, SPR_CSG_BEGIN + 66232, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kFlatToLeftBankBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kFlatToRightBankSprites = {{
        {{ SPR_CSG_BEGIN + 65451, SPR_CSG_BEGIN + 66207, SPR_CSG_BEGIN + 65486, SPR_CSG_BEGIN + 66242, }},
        {{ SPR_CSG_BEGIN + 65452, SPR_CSG_BEGIN + 66208, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65453, SPR_CSG_BEGIN + 66209, SPR_CSG_BEGIN + 65477, SPR_CSG_BEGIN + 66233, }},
        {{ SPR_CSG_BEGIN + 65454, SPR_CSG_BEGIN + 66210, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kFlatToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kFlatToLeftBankBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 7>, kNumOrthogonalDirections>
    kBankedRightQuarterTurn5TilesSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65585, SPR_CSG_BEGIN + 66341, SPR_CSG_BEGIN + 65605, SPR_CSG_BEGIN + 66361, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65586, SPR_CSG_BEGIN + 66342, SPR_CSG_BEGIN + 65606, SPR_CSG_BEGIN + 66362, }},
            {{ SPR_CSG_BEGIN + 65587, SPR_CSG_BEGIN + 66343, SPR_CSG_BEGIN + 65607, SPR_CSG_BEGIN + 66363, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65588, SPR_CSG_BEGIN + 66344, SPR_CSG_BEGIN + 65608, SPR_CSG_BEGIN + 66364, }},
            {{ SPR_CSG_BEGIN + 65589, SPR_CSG_BEGIN + 66345, SPR_CSG_BEGIN + 65609, SPR_CSG_BEGIN + 66365, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65590, SPR_CSG_BEGIN + 66346, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65591, SPR_CSG_BEGIN + 66347, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65592, SPR_CSG_BEGIN + 66348, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65593, SPR_CSG_BEGIN + 66349, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65594, SPR_CSG_BEGIN + 66350, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65595, SPR_CSG_BEGIN + 66351, SPR_CSG_BEGIN + 65610, SPR_CSG_BEGIN + 66366, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65596, SPR_CSG_BEGIN + 66352, SPR_CSG_BEGIN + 65611, SPR_CSG_BEGIN + 66367, }},
            {{ SPR_CSG_BEGIN + 65597, SPR_CSG_BEGIN + 66353, SPR_CSG_BEGIN + 65612, SPR_CSG_BEGIN + 66368, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65598, SPR_CSG_BEGIN + 66354, SPR_CSG_BEGIN + 65613, SPR_CSG_BEGIN + 66369, }},
            {{ SPR_CSG_BEGIN + 65599, SPR_CSG_BEGIN + 66355, SPR_CSG_BEGIN + 65614, SPR_CSG_BEGIN + 66370, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65600, SPR_CSG_BEGIN + 66356, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65601, SPR_CSG_BEGIN + 66357, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65602, SPR_CSG_BEGIN + 66358, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65603, SPR_CSG_BEGIN + 66359, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65604, SPR_CSG_BEGIN + 66360, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 7>, kNumOrthogonalDirections>
        kBankedRightQuarterTurn5TilesBoundBoxes = { {
            { {
                { { { { 0, 2, 0 }, { 32, 32, 2 } },
                    { { 0, 2, 0 }, { 32, 32, 2 } },
                    { { 0, 2, 27 }, { 32, 32, 0 } },
                    { { 0, 2, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 32, 32, 2 } },
                    { { 2, 0, 0 }, { 32, 32, 2 } },
                    { { 2, 0, 27 }, { 32, 32, 0 } },
                    { { 2, 0, 27 }, { 32, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 32, 32, 2 } }, { { 2, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } }, { { 0, 2, 0 }, { 32, 27, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 27 }, { 32, 27, 0 } },
                    { { 0, 2, 27 }, { 32, 27, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 27 }, { 32, 16, 0 } },
                    { { 0, 0, 27 }, { 32, 16, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } }, { { 2, 0, 0 }, { 27, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 2, 0 }, { 32, 32, 2 } }, { { 0, 2, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kLeftBankToUp25Sprites = {{
        {{ SPR_CSG_BEGIN + 65467, SPR_CSG_BEGIN + 66223, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65468, SPR_CSG_BEGIN + 66224, SPR_CSG_BEGIN + 65482, SPR_CSG_BEGIN + 66238, }},
        {{ SPR_CSG_BEGIN + 65469, SPR_CSG_BEGIN + 66225, SPR_CSG_BEGIN + 65483, SPR_CSG_BEGIN + 66239, }},
        {{ SPR_CSG_BEGIN + 65470, SPR_CSG_BEGIN + 66226, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kLeftBankToUp25BoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kRightBankToUp25Sprites = {{
        {{ SPR_CSG_BEGIN + 65471, SPR_CSG_BEGIN + 66227, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65472, SPR_CSG_BEGIN + 66228, SPR_CSG_BEGIN + 65484, SPR_CSG_BEGIN + 66240, }},
        {{ SPR_CSG_BEGIN + 65473, SPR_CSG_BEGIN + 66229, SPR_CSG_BEGIN + 65485, SPR_CSG_BEGIN + 66241, }},
        {{ SPR_CSG_BEGIN + 65474, SPR_CSG_BEGIN + 66230, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kRightBankToUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftBankToUp25BoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25ToLeftBankSprites = {{
        {{ SPR_CSG_BEGIN + 65459, SPR_CSG_BEGIN + 66215, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65460, SPR_CSG_BEGIN + 66216, SPR_CSG_BEGIN + 65478, SPR_CSG_BEGIN + 66234, }},
        {{ SPR_CSG_BEGIN + 65461, SPR_CSG_BEGIN + 66217, SPR_CSG_BEGIN + 65479, SPR_CSG_BEGIN + 66235, }},
        {{ SPR_CSG_BEGIN + 65462, SPR_CSG_BEGIN + 66218, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections> kUp25ToLeftBankBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } }, {}, {} } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 3, 0, 0 }, { 25, 32, 2 } },
            { { 26, 0, 5 }, { 1, 32, 9 } },
            { { 26, 0, 5 }, { 1, 32, 9 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 3, 0 }, { 32, 25, 2 } },
            { { 0, 26, 5 }, { 32, 1, 9 } },
            { { 0, 26, 5 }, { 32, 1, 9 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } }, {}, {} } },
    } };

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 4>, kNumOrthogonalDirections>
    kUp25ToRightBankSprites = {{
        {{ SPR_CSG_BEGIN + 65463, SPR_CSG_BEGIN + 66219, kImageIndexUndefined, kImageIndexUndefined, }},
        {{ SPR_CSG_BEGIN + 65464, SPR_CSG_BEGIN + 66220, SPR_CSG_BEGIN + 65480, SPR_CSG_BEGIN + 66236, }},
        {{ SPR_CSG_BEGIN + 65465, SPR_CSG_BEGIN + 66221, SPR_CSG_BEGIN + 65481, SPR_CSG_BEGIN + 66237, }},
        {{ SPR_CSG_BEGIN + 65466, SPR_CSG_BEGIN + 66222, kImageIndexUndefined, kImageIndexUndefined, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 4>, kNumOrthogonalDirections>
        kUp25ToRightBankBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kUp25ToLeftBankBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<ImageIndex, 2>, kNumOrthogonalDirections>
    kLeftBankSprites = {{
        {{ SPR_CSG_BEGIN + 65455, SPR_CSG_BEGIN + 66211, }},
        {{ SPR_CSG_BEGIN + 65456, SPR_CSG_BEGIN + 66212, }},
        {{ SPR_CSG_BEGIN + 65457, SPR_CSG_BEGIN + 66213, }},
        {{ SPR_CSG_BEGIN + 65458, SPR_CSG_BEGIN + 66214, }},
    }};
    // clang-format on

    static constexpr std::array<std::array<BoundBoxXYZ, 2>, kNumOrthogonalDirections> kLeftBankBoundBoxes = { {
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
        { { { { 0, 3, 0 }, { 32, 25, 2 } }, { { 0, 3, 0 }, { 32, 25, 2 } } } },
        { { { { 3, 0, 0 }, { 25, 32, 2 } }, { { 3, 0, 0 }, { 25, 32, 2 } } } },
    } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftBankedQuarterTurn3TilesSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65854, SPR_CSG_BEGIN + 66610, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65853, SPR_CSG_BEGIN + 66609, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65852, SPR_CSG_BEGIN + 66608, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65857, SPR_CSG_BEGIN + 66613, SPR_CSG_BEGIN + 65866, SPR_CSG_BEGIN + 66622, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65856, SPR_CSG_BEGIN + 66612, SPR_CSG_BEGIN + 65865, SPR_CSG_BEGIN + 66621, }},
            {{ SPR_CSG_BEGIN + 65855, SPR_CSG_BEGIN + 66611, SPR_CSG_BEGIN + 65864, SPR_CSG_BEGIN + 66620, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65860, SPR_CSG_BEGIN + 66616, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65859, SPR_CSG_BEGIN + 66615, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65858, SPR_CSG_BEGIN + 66614, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65851, SPR_CSG_BEGIN + 66607, SPR_CSG_BEGIN + 65863, SPR_CSG_BEGIN + 66619, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65850, SPR_CSG_BEGIN + 66606, SPR_CSG_BEGIN + 65862, SPR_CSG_BEGIN + 66618, }},
            {{ SPR_CSG_BEGIN + 65849, SPR_CSG_BEGIN + 66605, SPR_CSG_BEGIN + 65861, SPR_CSG_BEGIN + 66617, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankedQuarterTurn3TilesBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 8>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpSmallSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65890, SPR_CSG_BEGIN + 66646, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65889, SPR_CSG_BEGIN + 66645, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65888, SPR_CSG_BEGIN + 66644, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65887, SPR_CSG_BEGIN + 66643, SPR_CSG_BEGIN + 65899, SPR_CSG_BEGIN + 66655, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65886, SPR_CSG_BEGIN + 66642, SPR_CSG_BEGIN + 65898, SPR_CSG_BEGIN + 66654, }},
            {{ SPR_CSG_BEGIN + 65885, SPR_CSG_BEGIN + 66641, SPR_CSG_BEGIN + 65897, SPR_CSG_BEGIN + 66653, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65893, SPR_CSG_BEGIN + 66649, SPR_CSG_BEGIN + 65902, SPR_CSG_BEGIN + 66658, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65892, SPR_CSG_BEGIN + 66648, SPR_CSG_BEGIN + 65901, SPR_CSG_BEGIN + 66657, }},
            {{ SPR_CSG_BEGIN + 65891, SPR_CSG_BEGIN + 66647, SPR_CSG_BEGIN + 65900, SPR_CSG_BEGIN + 66656, }},
            {{ SPR_CSG_BEGIN + 65890, SPR_CSG_BEGIN + 66646, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65889, SPR_CSG_BEGIN + 66645, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65888, SPR_CSG_BEGIN + 66644, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65896, SPR_CSG_BEGIN + 66652, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65895, SPR_CSG_BEGIN + 66651, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65894, SPR_CSG_BEGIN + 66650, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65893, SPR_CSG_BEGIN + 66649, SPR_CSG_BEGIN + 65902, SPR_CSG_BEGIN + 66658, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65892, SPR_CSG_BEGIN + 66648, SPR_CSG_BEGIN + 65901, SPR_CSG_BEGIN + 66657, }},
            {{ SPR_CSG_BEGIN + 65891, SPR_CSG_BEGIN + 66647, SPR_CSG_BEGIN + 65900, SPR_CSG_BEGIN + 66656, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65887, SPR_CSG_BEGIN + 66643, SPR_CSG_BEGIN + 65899, SPR_CSG_BEGIN + 66655, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65886, SPR_CSG_BEGIN + 66642, SPR_CSG_BEGIN + 65898, SPR_CSG_BEGIN + 66654, }},
            {{ SPR_CSG_BEGIN + 65885, SPR_CSG_BEGIN + 66641, SPR_CSG_BEGIN + 65897, SPR_CSG_BEGIN + 66653, }},
            {{ SPR_CSG_BEGIN + 65896, SPR_CSG_BEGIN + 66652, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65895, SPR_CSG_BEGIN + 66651, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65894, SPR_CSG_BEGIN + 66650, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 8>, kNumOrthogonalDirections>
        kLeftHalfBankedHelixUpSmallBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 8>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpSmallSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65867, SPR_CSG_BEGIN + 66623, SPR_CSG_BEGIN + 65879, SPR_CSG_BEGIN + 66635, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65868, SPR_CSG_BEGIN + 66624, SPR_CSG_BEGIN + 65880, SPR_CSG_BEGIN + 66636, }},
            {{ SPR_CSG_BEGIN + 65869, SPR_CSG_BEGIN + 66625, SPR_CSG_BEGIN + 65881, SPR_CSG_BEGIN + 66637, }},
            {{ SPR_CSG_BEGIN + 65870, SPR_CSG_BEGIN + 66626, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65871, SPR_CSG_BEGIN + 66627, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65872, SPR_CSG_BEGIN + 66628, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65870, SPR_CSG_BEGIN + 66626, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65871, SPR_CSG_BEGIN + 66627, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65872, SPR_CSG_BEGIN + 66628, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65873, SPR_CSG_BEGIN + 66629, SPR_CSG_BEGIN + 65882, SPR_CSG_BEGIN + 66638, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65874, SPR_CSG_BEGIN + 66630, SPR_CSG_BEGIN + 65883, SPR_CSG_BEGIN + 66639, }},
            {{ SPR_CSG_BEGIN + 65875, SPR_CSG_BEGIN + 66631, SPR_CSG_BEGIN + 65884, SPR_CSG_BEGIN + 66640, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65873, SPR_CSG_BEGIN + 66629, SPR_CSG_BEGIN + 65882, SPR_CSG_BEGIN + 66638, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65874, SPR_CSG_BEGIN + 66630, SPR_CSG_BEGIN + 65883, SPR_CSG_BEGIN + 66639, }},
            {{ SPR_CSG_BEGIN + 65875, SPR_CSG_BEGIN + 66631, SPR_CSG_BEGIN + 65884, SPR_CSG_BEGIN + 66640, }},
            {{ SPR_CSG_BEGIN + 65876, SPR_CSG_BEGIN + 66632, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65877, SPR_CSG_BEGIN + 66633, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65878, SPR_CSG_BEGIN + 66634, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65876, SPR_CSG_BEGIN + 66632, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65877, SPR_CSG_BEGIN + 66633, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65878, SPR_CSG_BEGIN + 66634, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65867, SPR_CSG_BEGIN + 66623, SPR_CSG_BEGIN + 65879, SPR_CSG_BEGIN + 66635, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65868, SPR_CSG_BEGIN + 66624, SPR_CSG_BEGIN + 65880, SPR_CSG_BEGIN + 66636, }},
            {{ SPR_CSG_BEGIN + 65869, SPR_CSG_BEGIN + 66625, SPR_CSG_BEGIN + 65881, SPR_CSG_BEGIN + 66637, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 8>, kNumOrthogonalDirections>
        kRightHalfBankedHelixUpSmallBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpSmallBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 14>, kNumOrthogonalDirections>
    kLeftHalfBankedHelixUpLargeSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65654, SPR_CSG_BEGIN + 66410, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65653, SPR_CSG_BEGIN + 66409, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65652, SPR_CSG_BEGIN + 66408, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65651, SPR_CSG_BEGIN + 66407, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65650, SPR_CSG_BEGIN + 66406, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65649, SPR_CSG_BEGIN + 66405, SPR_CSG_BEGIN + 65669, SPR_CSG_BEGIN + 66425, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65648, SPR_CSG_BEGIN + 66404, SPR_CSG_BEGIN + 65668, SPR_CSG_BEGIN + 66424, }},
            {{ SPR_CSG_BEGIN + 65647, SPR_CSG_BEGIN + 66403, SPR_CSG_BEGIN + 65667, SPR_CSG_BEGIN + 66423, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65646, SPR_CSG_BEGIN + 66402, SPR_CSG_BEGIN + 65666, SPR_CSG_BEGIN + 66422, }},
            {{ SPR_CSG_BEGIN + 65645, SPR_CSG_BEGIN + 66401, SPR_CSG_BEGIN + 65665, SPR_CSG_BEGIN + 66421, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65659, SPR_CSG_BEGIN + 66415, SPR_CSG_BEGIN + 65674, SPR_CSG_BEGIN + 66430, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65658, SPR_CSG_BEGIN + 66414, SPR_CSG_BEGIN + 65673, SPR_CSG_BEGIN + 66429, }},
            {{ SPR_CSG_BEGIN + 65657, SPR_CSG_BEGIN + 66413, SPR_CSG_BEGIN + 65672, SPR_CSG_BEGIN + 66428, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65656, SPR_CSG_BEGIN + 66412, SPR_CSG_BEGIN + 65671, SPR_CSG_BEGIN + 66427, }},
            {{ SPR_CSG_BEGIN + 65655, SPR_CSG_BEGIN + 66411, SPR_CSG_BEGIN + 65670, SPR_CSG_BEGIN + 66426, }},
            {{ SPR_CSG_BEGIN + 65654, SPR_CSG_BEGIN + 66410, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65653, SPR_CSG_BEGIN + 66409, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65652, SPR_CSG_BEGIN + 66408, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65651, SPR_CSG_BEGIN + 66407, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65650, SPR_CSG_BEGIN + 66406, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65664, SPR_CSG_BEGIN + 66420, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65663, SPR_CSG_BEGIN + 66419, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65662, SPR_CSG_BEGIN + 66418, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65661, SPR_CSG_BEGIN + 66417, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65660, SPR_CSG_BEGIN + 66416, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65659, SPR_CSG_BEGIN + 66415, SPR_CSG_BEGIN + 65674, SPR_CSG_BEGIN + 66430, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65658, SPR_CSG_BEGIN + 66414, SPR_CSG_BEGIN + 65673, SPR_CSG_BEGIN + 66429, }},
            {{ SPR_CSG_BEGIN + 65657, SPR_CSG_BEGIN + 66413, SPR_CSG_BEGIN + 65672, SPR_CSG_BEGIN + 66428, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65656, SPR_CSG_BEGIN + 66412, SPR_CSG_BEGIN + 65671, SPR_CSG_BEGIN + 66427, }},
            {{ SPR_CSG_BEGIN + 65655, SPR_CSG_BEGIN + 66411, SPR_CSG_BEGIN + 65670, SPR_CSG_BEGIN + 66426, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65649, SPR_CSG_BEGIN + 66405, SPR_CSG_BEGIN + 65669, SPR_CSG_BEGIN + 66425, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65648, SPR_CSG_BEGIN + 66404, SPR_CSG_BEGIN + 65668, SPR_CSG_BEGIN + 66424, }},
            {{ SPR_CSG_BEGIN + 65647, SPR_CSG_BEGIN + 66403, SPR_CSG_BEGIN + 65667, SPR_CSG_BEGIN + 66423, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65646, SPR_CSG_BEGIN + 66402, SPR_CSG_BEGIN + 65666, SPR_CSG_BEGIN + 66422, }},
            {{ SPR_CSG_BEGIN + 65645, SPR_CSG_BEGIN + 66401, SPR_CSG_BEGIN + 65665, SPR_CSG_BEGIN + 66421, }},
            {{ SPR_CSG_BEGIN + 65664, SPR_CSG_BEGIN + 66420, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65663, SPR_CSG_BEGIN + 66419, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65662, SPR_CSG_BEGIN + 66418, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65661, SPR_CSG_BEGIN + 66417, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65660, SPR_CSG_BEGIN + 66416, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 14>, kNumOrthogonalDirections>
        kLeftHalfBankedHelixUpLargeBoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } }, { { 16, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
                { { { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 0 }, { 27, 32, 2 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } },
                    { { 2, 0, 27 }, { 27, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 0 }, { 16, 32, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } },
                    { { 16, 16, 29 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 0 }, { 32, 16, 2 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } },
                    { { 0, 0, 33 }, { 32, 16, 0 } } } },
                { { { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 0 }, { 32, 27, 2 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } },
                    { { 0, 2, 33 }, { 32, 27, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } },
                    { { 6, 0, 27 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 0 }, { 32, 16, 2 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } },
                    { { 0, 16, 27 }, { 32, 16, 0 } } } },
                { { { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 8 }, { 32, 20, 2 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } },
                    { { 0, 6, 27 }, { 32, 20, 0 } } } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 32, 2 } }, { { 0, 0, 0 }, { 16, 32, 2 } }, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 14>, kNumOrthogonalDirections>
    kRightHalfBankedHelixUpLargeSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65615, SPR_CSG_BEGIN + 66371, SPR_CSG_BEGIN + 65635, SPR_CSG_BEGIN + 66391, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65616, SPR_CSG_BEGIN + 66372, SPR_CSG_BEGIN + 65636, SPR_CSG_BEGIN + 66392, }},
            {{ SPR_CSG_BEGIN + 65617, SPR_CSG_BEGIN + 66373, SPR_CSG_BEGIN + 65637, SPR_CSG_BEGIN + 66393, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65618, SPR_CSG_BEGIN + 66374, SPR_CSG_BEGIN + 65638, SPR_CSG_BEGIN + 66394, }},
            {{ SPR_CSG_BEGIN + 65619, SPR_CSG_BEGIN + 66375, SPR_CSG_BEGIN + 65639, SPR_CSG_BEGIN + 66395, }},
            {{ SPR_CSG_BEGIN + 65620, SPR_CSG_BEGIN + 66376, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65621, SPR_CSG_BEGIN + 66377, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65622, SPR_CSG_BEGIN + 66378, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65623, SPR_CSG_BEGIN + 66379, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65624, SPR_CSG_BEGIN + 66380, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65620, SPR_CSG_BEGIN + 66376, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65621, SPR_CSG_BEGIN + 66377, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65622, SPR_CSG_BEGIN + 66378, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65623, SPR_CSG_BEGIN + 66379, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65624, SPR_CSG_BEGIN + 66380, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65625, SPR_CSG_BEGIN + 66381, SPR_CSG_BEGIN + 65640, SPR_CSG_BEGIN + 66396, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65626, SPR_CSG_BEGIN + 66382, SPR_CSG_BEGIN + 65641, SPR_CSG_BEGIN + 66397, }},
            {{ SPR_CSG_BEGIN + 65627, SPR_CSG_BEGIN + 66383, SPR_CSG_BEGIN + 65642, SPR_CSG_BEGIN + 66398, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65628, SPR_CSG_BEGIN + 66384, SPR_CSG_BEGIN + 65643, SPR_CSG_BEGIN + 66399, }},
            {{ SPR_CSG_BEGIN + 65629, SPR_CSG_BEGIN + 66385, SPR_CSG_BEGIN + 65644, SPR_CSG_BEGIN + 66400, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65625, SPR_CSG_BEGIN + 66381, SPR_CSG_BEGIN + 65640, SPR_CSG_BEGIN + 66396, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65626, SPR_CSG_BEGIN + 66382, SPR_CSG_BEGIN + 65641, SPR_CSG_BEGIN + 66397, }},
            {{ SPR_CSG_BEGIN + 65627, SPR_CSG_BEGIN + 66383, SPR_CSG_BEGIN + 65642, SPR_CSG_BEGIN + 66398, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65628, SPR_CSG_BEGIN + 66384, SPR_CSG_BEGIN + 65643, SPR_CSG_BEGIN + 66399, }},
            {{ SPR_CSG_BEGIN + 65629, SPR_CSG_BEGIN + 66385, SPR_CSG_BEGIN + 65644, SPR_CSG_BEGIN + 66400, }},
            {{ SPR_CSG_BEGIN + 65630, SPR_CSG_BEGIN + 66386, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65631, SPR_CSG_BEGIN + 66387, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65632, SPR_CSG_BEGIN + 66388, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65633, SPR_CSG_BEGIN + 66389, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65634, SPR_CSG_BEGIN + 66390, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65630, SPR_CSG_BEGIN + 66386, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65631, SPR_CSG_BEGIN + 66387, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65632, SPR_CSG_BEGIN + 66388, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65633, SPR_CSG_BEGIN + 66389, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65634, SPR_CSG_BEGIN + 66390, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65615, SPR_CSG_BEGIN + 66371, SPR_CSG_BEGIN + 65635, SPR_CSG_BEGIN + 66391, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65616, SPR_CSG_BEGIN + 66372, SPR_CSG_BEGIN + 65636, SPR_CSG_BEGIN + 66392, }},
            {{ SPR_CSG_BEGIN + 65617, SPR_CSG_BEGIN + 66373, SPR_CSG_BEGIN + 65637, SPR_CSG_BEGIN + 66393, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65618, SPR_CSG_BEGIN + 66374, SPR_CSG_BEGIN + 65638, SPR_CSG_BEGIN + 66394, }},
            {{ SPR_CSG_BEGIN + 65619, SPR_CSG_BEGIN + 66375, SPR_CSG_BEGIN + 65639, SPR_CSG_BEGIN + 66395, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 14>, kNumOrthogonalDirections>
        kRightHalfBankedHelixUpLargeBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftHalfBankedHelixUpLargeBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kLeftEighthBankToDiagSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 66163, SPR_CSG_BEGIN + 66919, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66164, SPR_CSG_BEGIN + 66920, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66165, SPR_CSG_BEGIN + 66921, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66166, SPR_CSG_BEGIN + 66922, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66167, SPR_CSG_BEGIN + 66923, SPR_CSG_BEGIN + 66179, SPR_CSG_BEGIN + 66935, }},
            {{ SPR_CSG_BEGIN + 66168, SPR_CSG_BEGIN + 66924, SPR_CSG_BEGIN + 66180, SPR_CSG_BEGIN + 66936, }},
            {{ SPR_CSG_BEGIN + 66169, SPR_CSG_BEGIN + 66925, SPR_CSG_BEGIN + 66181, SPR_CSG_BEGIN + 66937, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66170, SPR_CSG_BEGIN + 66926, SPR_CSG_BEGIN + 66182, SPR_CSG_BEGIN + 66938, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66171, SPR_CSG_BEGIN + 66927, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66172, SPR_CSG_BEGIN + 66928, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66173, SPR_CSG_BEGIN + 66929, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66174, SPR_CSG_BEGIN + 66930, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66175, SPR_CSG_BEGIN + 66931, SPR_CSG_BEGIN + 66183, SPR_CSG_BEGIN + 66939, }},
            {{ SPR_CSG_BEGIN + 66176, SPR_CSG_BEGIN + 66932, SPR_CSG_BEGIN + 66184, SPR_CSG_BEGIN + 66940, }},
            {{ SPR_CSG_BEGIN + 66177, SPR_CSG_BEGIN + 66933, SPR_CSG_BEGIN + 66185, SPR_CSG_BEGIN + 66941, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66178, SPR_CSG_BEGIN + 66934, SPR_CSG_BEGIN + 66186, SPR_CSG_BEGIN + 66942, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections>
        kLeftEighthBankToDiagBoundBoxes = { {
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { { { 0, 0, 0 }, { 32, 16, 2 } }, { { 0, 0, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } }, { { 0, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } }, { { 16, 16, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 34, 2 } },
                    { { 0, 0, 0 }, { 16, 34, 2 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } },
                    { { 0, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 0 }, { 16, 16, 2 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } },
                    { { 16, 16, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 16, 0, 0 }, { 18, 16, 2 } },
                    { { 16, 0, 0 }, { 18, 16, 2 } },
                    { { 16, 0, 27 }, { 16, 16, 0 } },
                    { { 16, 0, 27 }, { 16, 16, 0 } } } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } }, { { 0, 0, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { { { 0, 16, 0 }, { 32, 16, 2 } }, { { 0, 16, 0 }, { 32, 16, 2 } }, {}, {} } },
                { { { { 16, 0, 0 }, { 16, 16, 2 } }, { { 16, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } }, { { 0, 0, 0 }, { 16, 16, 2 } }, {}, {} } },
            } },
            { {
                { { { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 0 }, { 32, 32, 2 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } },
                    { { 0, 0, 27 }, { 32, 32, 0 } } } },
                { { { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 0 }, { 16, 32, 2 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } },
                    { { 16, 0, 27 }, { 16, 32, 0 } } } },
                { { { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 0 }, { 16, 16, 2 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } },
                    { { 0, 0, 27 }, { 16, 16, 0 } } } },
                { { {}, {}, {}, {} } },
                { { { { 0, 16, 0 }, { 16, 16, 2 } },
                    { { 0, 16, 0 }, { 16, 16, 2 } },
                    { { 0, 16, 27 }, { 16, 16, 0 } },
                    { { 0, 16, 27 }, { 16, 16, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 5>, kNumOrthogonalDirections>
    kRightEighthBankToDiagSprites = {{
        {{
            {{ SPR_CSG_BEGIN + 66139, SPR_CSG_BEGIN + 66895, SPR_CSG_BEGIN + 66155, SPR_CSG_BEGIN + 66911, }},
            {{ SPR_CSG_BEGIN + 66140, SPR_CSG_BEGIN + 66896, SPR_CSG_BEGIN + 66156, SPR_CSG_BEGIN + 66912, }},
            {{ SPR_CSG_BEGIN + 66141, SPR_CSG_BEGIN + 66897, SPR_CSG_BEGIN + 66157, SPR_CSG_BEGIN + 66913, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66142, SPR_CSG_BEGIN + 66898, SPR_CSG_BEGIN + 66158, SPR_CSG_BEGIN + 66914, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66143, SPR_CSG_BEGIN + 66899, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66144, SPR_CSG_BEGIN + 66900, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66145, SPR_CSG_BEGIN + 66901, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66146, SPR_CSG_BEGIN + 66902, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66147, SPR_CSG_BEGIN + 66903, SPR_CSG_BEGIN + 66159, SPR_CSG_BEGIN + 66915, }},
            {{ SPR_CSG_BEGIN + 66148, SPR_CSG_BEGIN + 66904, SPR_CSG_BEGIN + 66160, SPR_CSG_BEGIN + 66916, }},
            {{ SPR_CSG_BEGIN + 66149, SPR_CSG_BEGIN + 66905, SPR_CSG_BEGIN + 66161, SPR_CSG_BEGIN + 66917, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66150, SPR_CSG_BEGIN + 66906, SPR_CSG_BEGIN + 66162, SPR_CSG_BEGIN + 66918, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66151, SPR_CSG_BEGIN + 66907, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66152, SPR_CSG_BEGIN + 66908, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66153, SPR_CSG_BEGIN + 66909, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66154, SPR_CSG_BEGIN + 66910, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 5>, kNumOrthogonalDirections>
        kRightEighthBankToDiagBoundBoxes = flipTrackSequenceBoundBoxesXAxis(kLeftEighthBankToDiagBoundBoxes);

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagFlatToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66055, SPR_CSG_BEGIN + 66811, SPR_CSG_BEGIN + 66059, SPR_CSG_BEGIN + 66815, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66056, SPR_CSG_BEGIN + 66812, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66057, SPR_CSG_BEGIN + 66813, SPR_CSG_BEGIN + 66060, SPR_CSG_BEGIN + 66816, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66058, SPR_CSG_BEGIN + 66814, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToLeftBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToLeftBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagFlatToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66061, SPR_CSG_BEGIN + 66817, SPR_CSG_BEGIN + 66065, SPR_CSG_BEGIN + 66821, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66062, SPR_CSG_BEGIN + 66818, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66063, SPR_CSG_BEGIN + 66819, SPR_CSG_BEGIN + 66066, SPR_CSG_BEGIN + 66822, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66064, SPR_CSG_BEGIN + 66820, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToRightBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagFlatToRightBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66079, SPR_CSG_BEGIN + 66835, SPR_CSG_BEGIN + 66083, SPR_CSG_BEGIN + 66839, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66080, SPR_CSG_BEGIN + 66836, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66081, SPR_CSG_BEGIN + 66837, SPR_CSG_BEGIN + 66084, SPR_CSG_BEGIN + 66840, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66082, SPR_CSG_BEGIN + 66838, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagLeftBankToUp25Offsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagLeftBankToUp25BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagRightBankToUp25Sprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66085, SPR_CSG_BEGIN + 66841, SPR_CSG_BEGIN + 66089, SPR_CSG_BEGIN + 66845, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66086, SPR_CSG_BEGIN + 66842, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66087, SPR_CSG_BEGIN + 66843, SPR_CSG_BEGIN + 66090, SPR_CSG_BEGIN + 66846, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66088, SPR_CSG_BEGIN + 66844, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagRightBankToUp25Offsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagRightBankToUp25BoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66067, SPR_CSG_BEGIN + 66823, SPR_CSG_BEGIN + 66071, SPR_CSG_BEGIN + 66827, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66068, SPR_CSG_BEGIN + 66824, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66069, SPR_CSG_BEGIN + 66825, SPR_CSG_BEGIN + 66072, SPR_CSG_BEGIN + 66828, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66070, SPR_CSG_BEGIN + 66826, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToLeftBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToLeftBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagUp25ToRightBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66073, SPR_CSG_BEGIN + 66829, SPR_CSG_BEGIN + 66077, SPR_CSG_BEGIN + 66833, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66074, SPR_CSG_BEGIN + 66830, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66075, SPR_CSG_BEGIN + 66831, SPR_CSG_BEGIN + 66078, SPR_CSG_BEGIN + 66834, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66076, SPR_CSG_BEGIN + 66832, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToRightBankOffsets = { {
            { {
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kDiagUp25ToRightBankBoundBoxes = { {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } },
                    { { -16, -16, 35 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kDiagLeftBankSprites = {{
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66049, SPR_CSG_BEGIN + 66805, SPR_CSG_BEGIN + 66053, SPR_CSG_BEGIN + 66809, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66050, SPR_CSG_BEGIN + 66806, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 66051, SPR_CSG_BEGIN + 66807, SPR_CSG_BEGIN + 66054, SPR_CSG_BEGIN + 66810, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 66052, SPR_CSG_BEGIN + 66808, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagLeftBankOffsets = { {
        { {
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
        } },
        { {
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 }, { -16, -16, 0 } } },
            { { {}, {}, {}, {} } },
        } },
        { {
            { { { -16, -16, 0 }, { -16, -16, 0 }, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
            { { {}, {}, {}, {} } },
        } },
    } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections> kDiagLeftBankBoundBoxes = {
        {
            { {
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
            } },
            { {
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 0 }, { 32, 32, 2 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } },
                    { { -16, -16, 27 }, { 32, 32, 0 } } } },
                { { {}, {}, {}, {} } },
            } },
            { {
                { { { { -16, -16, 0 }, { 32, 32, 2 } }, { { -16, -16, 0 }, { 32, 32, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
            } },
        }
    };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kLeftBankToLeftQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65966, SPR_CSG_BEGIN + 66722, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65965, SPR_CSG_BEGIN + 66721, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65968, SPR_CSG_BEGIN + 66724, SPR_CSG_BEGIN + 65979, SPR_CSG_BEGIN + 66735, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65967, SPR_CSG_BEGIN + 66723, SPR_CSG_BEGIN + 65978, SPR_CSG_BEGIN + 66734, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65970, SPR_CSG_BEGIN + 66726, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65969, SPR_CSG_BEGIN + 66725, SPR_CSG_BEGIN + 65980, SPR_CSG_BEGIN + 66736, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65964, SPR_CSG_BEGIN + 66720, SPR_CSG_BEGIN + 65977, SPR_CSG_BEGIN + 66733, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65963, SPR_CSG_BEGIN + 66719, SPR_CSG_BEGIN + 65976, SPR_CSG_BEGIN + 66732, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankToLeftQuarterTurn3TilesUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes = { {
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } }, { { 6, 0, 0 }, { 20, 32, 2 } }, {}, {} } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
            { {
                { { { { 0, 6, 0 }, { 32, 20, 2 } }, { { 0, 6, 0 }, { 32, 20, 2 } }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
            } },
            { {
                { { { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 0 }, { 20, 32, 2 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } },
                    { { 6, 0, 67 }, { 20, 32, 0 } } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 0 }, { 32, 20, 2 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } },
                    { { 0, 6, 67 }, { 32, 20, 0 } } } },
            } },
        } };

    // clang-format off
    static constexpr std::array<std::array<std::array<ImageIndex, 4>, 4>, kNumOrthogonalDirections>
    kRightBankToRightQuarterTurn3TilesUp25Sprites = {{
        {{
            {{ SPR_CSG_BEGIN + 65955, SPR_CSG_BEGIN + 66711, SPR_CSG_BEGIN + 65971, SPR_CSG_BEGIN + 66727, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65956, SPR_CSG_BEGIN + 66712, SPR_CSG_BEGIN + 65972, SPR_CSG_BEGIN + 66728, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65957, SPR_CSG_BEGIN + 66713, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65958, SPR_CSG_BEGIN + 66714, SPR_CSG_BEGIN + 65973, SPR_CSG_BEGIN + 66729, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65959, SPR_CSG_BEGIN + 66715, SPR_CSG_BEGIN + 65974, SPR_CSG_BEGIN + 66730, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65960, SPR_CSG_BEGIN + 66716, SPR_CSG_BEGIN + 65975, SPR_CSG_BEGIN + 66731, }},
        }},
        {{
            {{ SPR_CSG_BEGIN + 65961, SPR_CSG_BEGIN + 66717, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, }},
            {{ SPR_CSG_BEGIN + 65962, SPR_CSG_BEGIN + 66718, kImageIndexUndefined, kImageIndexUndefined, }},
        }},
    }};
    // clang-format on

    static constexpr std::array<std::array<std::array<CoordsXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightBankToRightQuarterTurn3TilesUp25Offsets = { {
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
            } },
            { {
                { { { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 }, { 0, 6, 0 } } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 }, { 6, 0, 0 } } },
            } },
            { {
                { { { 6, 0, 0 }, { 6, 0, 0 }, {}, {} } },
                { { {}, {}, {}, {} } },
                { { {}, {}, {}, {} } },
                { { { 0, 6, 0 }, { 0, 6, 0 }, {}, {} } },
            } },
        } };

    static constexpr std::array<std::array<std::array<BoundBoxXYZ, 4>, 4>, kNumOrthogonalDirections>
        kRightBankToRightQuarterTurn3TilesUp25BoundBoxes = flipTrackSequenceBoundBoxesXAxis(
            kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes);

    constexpr std::array<TrackElementSprites, EnumValue(OpenRCT2::TrackElemType::Count)> kSprites = { {
        { &OpenRCT2::WoodenRollerCoaster::kFlatSprites[0][0][0], &OpenRCT2::WoodenRollerCoaster::kFlatOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kFlatBoundBoxes[0][0], false }, // Flat
        { &OpenRCT2::WoodenRollerCoaster::kEndStationSprites[0][0][0], &OpenRCT2::WoodenRollerCoaster::kEndStationOffsets[0],
          &OpenRCT2::WoodenRollerCoaster::kEndStationBoundBoxes[0], false }, // EndStation
        { &OpenRCT2::WoodenRollerCoaster::kStationSprites[0][0], &OpenRCT2::WoodenRollerCoaster::kStationOffsets[0],
          &OpenRCT2::WoodenRollerCoaster::kStationBoundBoxes[0], false }, // BeginStation
        { &OpenRCT2::WoodenRollerCoaster::kStationSprites[0][0], &OpenRCT2::WoodenRollerCoaster::kStationOffsets[0],
          &OpenRCT2::WoodenRollerCoaster::kStationBoundBoxes[0], false }, // MiddleStation
        { &OpenRCT2::WoodenRollerCoaster::kUp25Sprites[0][0][0], nullptr, &OpenRCT2::WoodenRollerCoaster::kUp25BoundBoxes[0][0],
          false }, // Up25
        { &OpenRCT2::WoodenRollerCoaster::kUp60Sprites[0][0][0], nullptr, &OpenRCT2::WoodenRollerCoaster::kUp60BoundBoxes[0][0],
          false }, // Up60
        { &OpenRCT2::WoodenRollerCoaster::kFlatToUp25Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kFlatToUp25BoundBoxes[0][0], false }, // FlatToUp25
        { &OpenRCT2::WoodenRollerCoaster::kUp25ToUp60Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp25ToUp60BoundBoxes[0][0], false }, // Up25ToUp60
        { &OpenRCT2::WoodenRollerCoaster::kUp60ToUp25Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp60ToUp25BoundBoxes[0][0], false }, // Up60ToUp25
        { &OpenRCT2::WoodenRollerCoaster::kUp25ToFlatSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp25ToFlatBoundBoxes[0][0], false }, // Up25ToFlat
        { nullptr, nullptr, nullptr, true },                                    // Down25
        { nullptr, nullptr, nullptr, true },                                    // Down60
        { nullptr, nullptr, nullptr, true },                                    // FlatToDown25
        { nullptr, nullptr, nullptr, true },                                    // Down25ToDown60
        { nullptr, nullptr, nullptr, true },                                    // Down60ToDown25
        { nullptr, nullptr, nullptr, true },                                    // Down25ToFlat
        { nullptr, nullptr, nullptr, true },                                    // LeftQuarterTurn5Tiles
        { &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn5TilesSprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn5TilesOffsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn5TilesBoundBoxes[0][0][0], false }, // RightQuarterTurn5Tiles
        { &kFlatToLeftBankSprites[0][0], nullptr, &kFlatToLeftBankBoundBoxes[0][0], false },   // FlatToLeftBank
        { &kFlatToRightBankSprites[0][0], nullptr, &kFlatToRightBankBoundBoxes[0][0], false }, // FlatToRightBank
        { nullptr, nullptr, nullptr, true },                                                   // LeftBankToFlat
        { nullptr, nullptr, nullptr, true },                                                   // RightBankToFlat
        { nullptr, nullptr, nullptr, true },                                                   // BankedLeftQuarterTurn5Tiles
        { &kBankedRightQuarterTurn5TilesSprites[0][0][0], nullptr, &kBankedRightQuarterTurn5TilesBoundBoxes[0][0][0],
          false },                                                                             // BankedRightQuarterTurn5Tiles
        { &kLeftBankToUp25Sprites[0][0], nullptr, &kLeftBankToUp25BoundBoxes[0][0], false },   // LeftBankToUp25
        { &kRightBankToUp25Sprites[0][0], nullptr, &kRightBankToUp25BoundBoxes[0][0], false }, // RightBankToUp25
        { &kUp25ToLeftBankSprites[0][0], nullptr, &kUp25ToLeftBankBoundBoxes[0][0], false },   // Up25ToLeftBank
        { &kUp25ToRightBankSprites[0][0], nullptr, &kUp25ToRightBankBoundBoxes[0][0], false }, // Up25ToRightBank
        { nullptr, nullptr, nullptr, true },                                                   // LeftBankToDown25
        { nullptr, nullptr, nullptr, true },                                                   // RightBankToDown25
        { nullptr, nullptr, nullptr, true },                                                   // Down25ToLeftBank
        { nullptr, nullptr, nullptr, true },                                                   // Down25ToRightBank
        { &kLeftBankSprites[0][0], nullptr, &kLeftBankBoundBoxes[0][0], false },               // LeftBank
        { nullptr, nullptr, nullptr, true },                                                   // RightBank
        { &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn5TilesUp25Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn5TilesUp25BoundBoxes[0][0][0], false }, // LeftQuarterTurn5TilesUp25
        { &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn5TilesUp25Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn5TilesUp25BoundBoxes[0][0][0], false }, // RightQuarterTurn5TilesUp25
        { nullptr, nullptr, nullptr, true }, // LeftQuarterTurn5TilesDown25
        { nullptr, nullptr, nullptr, true }, // RightQuarterTurn5TilesDown25
        { &OpenRCT2::WoodenRollerCoaster::kSBendLeftSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kSBendLeftBoundBoxes[0][0][0], false }, // SBendLeft
        { &OpenRCT2::WoodenRollerCoaster::kSBendRightSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kSBendRightBoundBoxes[0][0][0], false }, // SBendRight
        { &OpenRCT2::WoodenRollerCoaster::kLeftVerticalLoopSprites[0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftVerticalLoopOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftVerticalLoopBoundBoxes[0][0], false }, // LeftVerticalLoop
        { &OpenRCT2::WoodenRollerCoaster::kRightVerticalLoopSprites[0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightVerticalLoopOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightVerticalLoopBoundBoxes[0][0], false }, // RightVerticalLoop
        { &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn3TilesSprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn3TilesOffsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn3TilesBoundBoxes[0][0][0], false }, // LeftQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true },                                                  // RightQuarterTurn3Tiles
        { &kLeftBankedQuarterTurn3TilesSprites[0][0][0], nullptr, &kLeftBankedQuarterTurn3TilesBoundBoxes[0][0][0],
          false },                           // LeftBankedQuarterTurn3Tiles
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3Tiles
        { &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn3TilesUp25Sprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn3TilesUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftQuarterTurn3TilesUp25
        { &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn3TilesUp25Sprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn3TilesUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true },  // LeftQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, true },  // RightQuarterTurn3TilesDown25
        { nullptr, nullptr, nullptr, false }, // LeftQuarterTurn1Tile
        { nullptr, nullptr, nullptr, true },  // RightQuarterTurn1Tile
        { nullptr, nullptr, nullptr, false }, // LeftTwistDownToUp
        { nullptr, nullptr, nullptr, false }, // RightTwistDownToUp
        { nullptr, nullptr, nullptr, true },  // LeftTwistUpToDown
        { nullptr, nullptr, nullptr, true },  // RightTwistUpToDown
        { nullptr, nullptr, nullptr, false }, // HalfLoopUp
        { nullptr, nullptr, nullptr, true },  // HalfLoopDown
        { nullptr, nullptr, nullptr, false }, // LeftCorkscrewUp
        { nullptr, nullptr, nullptr, false }, // RightCorkscrewUp
        { nullptr, nullptr, nullptr, true },  // LeftCorkscrewDown
        { nullptr, nullptr, nullptr, true },  // RightCorkscrewDown
        { nullptr, nullptr, nullptr, false }, // FlatToUp60
        { nullptr, nullptr, nullptr, false }, // Up60ToFlat
        { nullptr, nullptr, nullptr, true },  // FlatToDown60
        { nullptr, nullptr, nullptr, true },  // Down60ToFlat
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
        { &kLeftHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpSmallBoundBoxes[0][0][0],
          false }, // LeftHalfBankedHelixUpSmall
        { &kRightHalfBankedHelixUpSmallSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpSmallBoundBoxes[0][0][0],
          false },                           // RightHalfBankedHelixUpSmall
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownSmall
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownSmall
        { &kLeftHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kLeftHalfBankedHelixUpLargeBoundBoxes[0][0][0],
          false }, // LeftHalfBankedHelixUpLarge
        { &kRightHalfBankedHelixUpLargeSprites[0][0][0], nullptr, &kRightHalfBankedHelixUpLargeBoundBoxes[0][0][0],
          false },                           // RightHalfBankedHelixUpLarge
        { nullptr, nullptr, nullptr, true }, // LeftHalfBankedHelixDownLarge
        { nullptr, nullptr, nullptr, true }, // RightHalfBankedHelixDownLarge
        { &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn1TileUp60Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftQuarterTurn1TileUp60BoundBoxes[0][0], false }, // LeftQuarterTurn1TileUp60
        { &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn1TileUp60Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightQuarterTurn1TileUp60BoundBoxes[0][0], false }, // RightQuarterTurn1TileUp60
        { nullptr, nullptr, nullptr, true },                                                   // LeftQuarterTurn1TileDown60
        { nullptr, nullptr, nullptr, true },                                                   // RightQuarterTurn1TileDown60
        { &OpenRCT2::WoodenRollerCoaster::kBrakesSprites[0][0], &OpenRCT2::WoodenRollerCoaster::kBrakesOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kBrakesBoundBoxes[0][0], false }, // Brakes
        { &OpenRCT2::WoodenRollerCoaster::kBoosterSprites[0][0], &OpenRCT2::WoodenRollerCoaster::kBoosterOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kBoosterBoundBoxes[0][0], false }, // Booster
        { nullptr, nullptr, nullptr, false },                                // Maze
        { nullptr, nullptr, nullptr, false },                                // LeftQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, false },                                // RightQuarterBankedHelixLargeUp
        { nullptr, nullptr, nullptr, true },                                 // LeftQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, true },                                 // RightQuarterBankedHelixLargeDown
        { nullptr, nullptr, nullptr, false },                                // LeftQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, false },                                // RightQuarterHelixLargeUp
        { nullptr, nullptr, nullptr, true },                                 // LeftQuarterHelixLargeDown
        { nullptr, nullptr, nullptr, true },                                 // RightQuarterHelixLargeDown
        { &OpenRCT2::WoodenRollerCoaster::kUp25LeftBankedSprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp25LeftBankedBoundBoxes[0][0], false }, // Up25LeftBanked
        { &OpenRCT2::WoodenRollerCoaster::kUp25RightBankedSprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp25RightBankedBoundBoxes[0][0], false }, // Up25RightBanked
        { nullptr, nullptr, nullptr, false },                                        // Waterfall
        { nullptr, nullptr, nullptr, false },                                        // Rapids
        { &OpenRCT2::WoodenRollerCoaster::kFlatSprites[0][0][0], &OpenRCT2::WoodenRollerCoaster::kFlatOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kFlatBoundBoxes[0][0], false }, // OnRidePhoto
        { nullptr, nullptr, nullptr, true },                              // Down25LeftBanked
        { nullptr, nullptr, nullptr, true },                              // Down25RightBanked
        { &OpenRCT2::WoodenRollerCoaster::kWatersplashSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kWatersplashBoundBoxes[0][0][0], false }, // Watersplash
        { &OpenRCT2::WoodenRollerCoaster::kFlatToUp60LongBaseSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kFlatToUp60LongBaseBoundBoxes[0][0][0], false }, // FlatToUp60LongBase
        { &OpenRCT2::WoodenRollerCoaster::kUp60ToFlatLongBaseSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp60ToFlatLongBaseBoundBoxes[0][0][0], false }, // Up60ToFlatLongBase
        { nullptr, nullptr, nullptr, false },                                              // Whirlpool
        { nullptr, nullptr, nullptr, true },                                               // Down60ToFlatLongBase
        { nullptr, nullptr, nullptr, true },                                               // FlatToDown60LongBase
        { nullptr, nullptr, nullptr, false },                                              // CableLiftHill
        { nullptr, nullptr, nullptr, false },                                              // ReverseFreefallSlope
        { nullptr, nullptr, nullptr, false },                                              // ReverseFreefallVertical
        { nullptr, nullptr, nullptr, false },                                              // Up90
        { nullptr, nullptr, nullptr, true },                                               // Down90
        { nullptr, nullptr, nullptr, false },                                              // Up60ToUp90
        { nullptr, nullptr, nullptr, true },                                               // Down90ToDown60
        { nullptr, nullptr, nullptr, false },                                              // Up90ToUp60
        { nullptr, nullptr, nullptr, true },                                               // Down60ToDown90
        { nullptr, nullptr, nullptr, false },                                              // BrakeForDrop
        { &OpenRCT2::WoodenRollerCoaster::kLeftEighthToDiagSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftEighthToDiagBoundBoxes[0][0][0], false }, // LeftEighthToDiag
        { &OpenRCT2::WoodenRollerCoaster::kRightEighthToDiagSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightEighthToDiagBoundBoxes[0][0][0], false }, // RightEighthToDiag
        { nullptr, nullptr, nullptr, true },                                              // LeftEighthToOrthogonal
        { nullptr, nullptr, nullptr, true },                                              // RightEighthToOrthogonal
        { &kLeftEighthBankToDiagSprites[0][0][0], nullptr, &kLeftEighthBankToDiagBoundBoxes[0][0][0],
          false }, // LeftEighthBankToDiag
        { &kRightEighthBankToDiagSprites[0][0][0], nullptr, &kRightEighthBankToDiagBoundBoxes[0][0][0],
          false },                           // RightEighthBankToDiag
        { nullptr, nullptr, nullptr, true }, // LeftEighthBankToOrthogonal
        { nullptr, nullptr, nullptr, true }, // RightEighthBankToOrthogonal
        { &OpenRCT2::WoodenRollerCoaster::kDiagFlatSprites[0][0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagFlatOffsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagFlatBoundBoxes[0][0][0], false }, // DiagFlat
        { &OpenRCT2::WoodenRollerCoaster::kDiagUp25Sprites[0][0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp25BoundBoxes[0][0][0], false }, // DiagUp25
        { &OpenRCT2::WoodenRollerCoaster::kDiagUp60Sprites[0][0][0], &OpenRCT2::WoodenRollerCoaster::kDiagUp60Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp60BoundBoxes[0][0][0], false }, // DiagUp60
        { &OpenRCT2::WoodenRollerCoaster::kDiagFlatToUp25Sprites[0][0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagFlatToUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagFlatToUp25BoundBoxes[0][0][0], false }, // DiagFlatToUp25
        { &OpenRCT2::WoodenRollerCoaster::kDiagUp25ToUp60Sprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp25ToUp60Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp25ToUp60BoundBoxes[0][0][0], false }, // DiagUp25ToUp60
        { &OpenRCT2::WoodenRollerCoaster::kDiagUp60ToUp25Sprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp60ToUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp60ToUp25BoundBoxes[0][0][0], false }, // DiagUp60ToUp25
        { &OpenRCT2::WoodenRollerCoaster::kDiagUp25ToFlatSprites[0][0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp25ToFlatOffsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagUp25ToFlatBoundBoxes[0][0][0], false }, // DiagUp25ToFlat
        { nullptr, nullptr, nullptr, true },                                           // DiagDown25
        { nullptr, nullptr, nullptr, true },                                           // DiagDown60
        { nullptr, nullptr, nullptr, true },                                           // DiagFlatToDown25
        { nullptr, nullptr, nullptr, true },                                           // DiagDown25ToDown60
        { nullptr, nullptr, nullptr, true },                                           // DiagDown60ToDown25
        { nullptr, nullptr, nullptr, true },                                           // DiagDown25ToFlat
        { nullptr, nullptr, nullptr, false },                                          // DiagFlatToUp60
        { nullptr, nullptr, nullptr, false },                                          // DiagUp60ToFlat
        { nullptr, nullptr, nullptr, true },                                           // DiagFlatToDown60
        { nullptr, nullptr, nullptr, true },                                           // DiagDown60ToFlat
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
        { &kDiagLeftBankSprites[0][0][0], &kDiagLeftBankOffsets[0][0][0], &kDiagLeftBankBoundBoxes[0][0][0],
          false },                            // DiagLeftBank
        { nullptr, nullptr, nullptr, true },  // DiagRightBank
        { nullptr, nullptr, nullptr, false }, // LogFlumeReverser
        { nullptr, nullptr, nullptr, false }, // SpinningTunnel
        { nullptr, nullptr, nullptr, false }, // LeftBarrelRollUpToDown
        { nullptr, nullptr, nullptr, false }, // RightBarrelRollUpToDown
        { nullptr, nullptr, nullptr, true },  // LeftBarrelRollDownToUp
        { nullptr, nullptr, nullptr, true },  // RightBarrelRollDownToUp
        { &kLeftBankToLeftQuarterTurn3TilesUp25Sprites[0][0][0], &kLeftBankToLeftQuarterTurn3TilesUp25Offsets[0][0][0],
          &kLeftBankToLeftQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // LeftBankToLeftQuarterTurn3TilesUp25
        { &kRightBankToRightQuarterTurn3TilesUp25Sprites[0][0][0], &kRightBankToRightQuarterTurn3TilesUp25Offsets[0][0][0],
          &kRightBankToRightQuarterTurn3TilesUp25BoundBoxes[0][0][0], false }, // RightBankToRightQuarterTurn3TilesUp25
        { nullptr, nullptr, nullptr, true },                                   // LeftQuarterTurn3TilesDown25ToLeftBank
        { nullptr, nullptr, nullptr, true },                                   // RightQuarterTurn3TilesDown25ToRightBank
        { nullptr, nullptr, nullptr, false },                                  // PoweredLift
        { &OpenRCT2::WoodenRollerCoaster::kLeftLargeHalfLoopUpSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftLargeHalfLoopUpBoundBoxes[0][0][0], false }, // LeftLargeHalfLoopUp
        { &OpenRCT2::WoodenRollerCoaster::kRightLargeHalfLoopUpSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightLargeHalfLoopUpBoundBoxes[0][0][0], false }, // RightLargeHalfLoopUp
        { nullptr, nullptr, nullptr, true },                                                 // LeftLargeHalfLoopDown
        { nullptr, nullptr, nullptr, true },                                                 // RightLargeHalfLoopDown
        { nullptr, nullptr, nullptr, false },                                                // LeftFlyerTwistUp
        { nullptr, nullptr, nullptr, false },                                                // RightFlyerTwistUp
        { nullptr, nullptr, nullptr, true },                                                 // LeftFlyerTwistDown
        { nullptr, nullptr, nullptr, true },                                                 // RightFlyerTwistDown
        { nullptr, nullptr, nullptr, false },                                                // FlyerHalfLoopUninvertedUp
        { nullptr, nullptr, nullptr, false },                                                // FlyerHalfLoopInvertedDown
        { nullptr, nullptr, nullptr, true },                                                 // LeftFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, true },                                                 // RightFlyerCorkscrewUp
        { nullptr, nullptr, nullptr, false },                                                // LeftFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false },                                                // RightFlyerCorkscrewDown
        { nullptr, nullptr, nullptr, false },                                                // HeartLineTransferUp
        { nullptr, nullptr, nullptr, false },                                                // HeartLineTransferDown
        { nullptr, nullptr, nullptr, false },                                                // LeftHeartLineRoll
        { nullptr, nullptr, nullptr, false },                                                // RightHeartLineRoll
        { nullptr, nullptr, nullptr, false },                                                // MinigolfHoleA
        { nullptr, nullptr, nullptr, false },                                                // MinigolfHoleB
        { nullptr, nullptr, nullptr, false },                                                // MinigolfHoleC
        { nullptr, nullptr, nullptr, false },                                                // MinigolfHoleD
        { nullptr, nullptr, nullptr, false },                                                // MinigolfHoleE
        { nullptr, nullptr, nullptr, false }, // MultiDimInvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // Up90ToInvertedFlatQuarterLoop
        { nullptr, nullptr, nullptr, false }, // InvertedFlatToDown90QuarterLoop
        { nullptr, nullptr, nullptr, false }, // LeftCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // RightCurvedLiftHill
        { nullptr, nullptr, nullptr, false }, // LeftReverser
        { nullptr, nullptr, nullptr, true },  // RightReverser
        { nullptr, nullptr, nullptr, false }, // AirThrustTopCap
        { nullptr, nullptr, nullptr, true },  // AirThrustVerticalDown
        { nullptr, nullptr, nullptr, true },  // AirThrustVerticalDownToLevel
        { &OpenRCT2::WoodenRollerCoaster::kBlockBrakesSprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kBlockBrakesOffsets[0][0],
          &OpenRCT2::WoodenRollerCoaster::kBlockBrakesBoundBoxes[0][0], false }, // BlockBrakes
        { &OpenRCT2::WoodenRollerCoaster::kLeftBankedQuarterTurn3TileUp25Sprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedQuarterTurn3TileUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedQuarterTurn3TileUp25BoundBoxes[0][0][0],
          false }, // LeftBankedQuarterTurn3TileUp25
        { &OpenRCT2::WoodenRollerCoaster::kRightBankedQuarterTurn3TileUp25Sprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightBankedQuarterTurn3TileUp25Offsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kRightBankedQuarterTurn3TileUp25BoundBoxes[0][0][0],
          false },                           // RightBankedQuarterTurn3TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn3TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn3TileDown25
        { &OpenRCT2::WoodenRollerCoaster::kLeftBankedQuarterTurn5TileUp25Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedQuarterTurn5TileUp25BoundBoxes[0][0][0],
          false }, // LeftBankedQuarterTurn5TileUp25
        { &OpenRCT2::WoodenRollerCoaster::kRightBankedQuarterTurn5TileUp25Sprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightBankedQuarterTurn5TileUp25BoundBoxes[0][0][0],
          false },                           // RightBankedQuarterTurn5TileUp25
        { nullptr, nullptr, nullptr, true }, // LeftBankedQuarterTurn5TileDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedQuarterTurn5TileDown25
        { &OpenRCT2::WoodenRollerCoaster::kUp25ToLeftBankedUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp25ToLeftBankedUp25BoundBoxes[0][0], false }, // Up25ToLeftBankedUp25
        { &OpenRCT2::WoodenRollerCoaster::kUp25ToRightBankedUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kUp25ToRightBankedUp25BoundBoxes[0][0], false }, // Up25ToRightBankedUp25
        { &OpenRCT2::WoodenRollerCoaster::kLeftBankedUp25ToUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedUp25ToUp25BoundBoxes[0][0], false }, // LeftBankedUp25ToUp25
        { &OpenRCT2::WoodenRollerCoaster::kRightBankedUp25ToUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightBankedUp25ToUp25BoundBoxes[0][0], false }, // RightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true },                                               // Down25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                               // Down25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                               // LeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true },                                               // RightBankedDown25ToDown25
        { &OpenRCT2::WoodenRollerCoaster::kLeftBankedFlatToLeftBankedUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedFlatToLeftBankedUp25BoundBoxes[0][0],
          false }, // LeftBankedFlatToLeftBankedUp25
        { &OpenRCT2::WoodenRollerCoaster::kRightBankedFlatToRightBankedUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightBankedFlatToRightBankedUp25BoundBoxes[0][0],
          false }, // RightBankedFlatToRightBankedUp25
        { &OpenRCT2::WoodenRollerCoaster::kLeftBankedUp25ToLeftBankedFlatSprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedUp25ToLeftBankedFlatBoundBoxes[0][0],
          false }, // LeftBankedUp25ToLeftBankedFlat
        { &OpenRCT2::WoodenRollerCoaster::kRightBankedUp25ToRightBankedFlatSprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightBankedUp25ToRightBankedFlatBoundBoxes[0][0],
          false },                           // RightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true }, // LeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true }, // RightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true }, // LeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true }, // RightBankedDown25ToRightBankedFlat
        { &OpenRCT2::WoodenRollerCoaster::kFlatToLeftBankedUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kFlatToLeftBankedUp25BoundBoxes[0][0], false }, // FlatToLeftBankedUp25
        { &OpenRCT2::WoodenRollerCoaster::kFlatToRightBankedUp25Sprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kFlatToRightBankedUp25BoundBoxes[0][0], false }, // FlatToRightBankedUp25
        { &OpenRCT2::WoodenRollerCoaster::kLeftBankedUp25ToFlatSprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftBankedUp25ToFlatBoundBoxes[0][0], false }, // LeftBankedUp25ToFlat
        { &OpenRCT2::WoodenRollerCoaster::kRightBankedUp25ToFlatSprites[0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightBankedUp25ToFlatBoundBoxes[0][0], false }, // RightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true },                                               // FlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },                                               // FlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },                                               // LeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true },                                               // RightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false },                                              // LeftQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, false },                                              // RightQuarterTurn1TileUp90
        { nullptr, nullptr, nullptr, true },                                               // LeftQuarterTurn1TileDown90
        { nullptr, nullptr, nullptr, true },                                               // RightQuarterTurn1TileDown90
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
        { nullptr, nullptr, nullptr, true },  // LeftLargeCorkscrewDown
        { nullptr, nullptr, nullptr, true },  // RightLargeCorkscrewDown
        { &OpenRCT2::WoodenRollerCoaster::kLeftMediumHalfLoopUpSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kLeftMediumHalfLoopUpBoundBoxes[0][0][0], false }, // LeftMediumHalfLoopUp
        { &OpenRCT2::WoodenRollerCoaster::kRightMediumHalfLoopUpSprites[0][0][0], nullptr,
          &OpenRCT2::WoodenRollerCoaster::kRightMediumHalfLoopUpBoundBoxes[0][0][0], false }, // RightMediumHalfLoopUp
        { nullptr, nullptr, nullptr, true },                                                  // LeftMediumHalfLoopDown
        { nullptr, nullptr, nullptr, true },                                                  // RightMediumHalfLoopDown
        { nullptr, nullptr, nullptr, false },                                                 // LeftZeroGRollUp
        { nullptr, nullptr, nullptr, false },                                                 // RightZeroGRollUp
        { nullptr, nullptr, nullptr, true },                                                  // LeftZeroGRollDown
        { nullptr, nullptr, nullptr, true },                                                  // RightZeroGRollDown
        { nullptr, nullptr, nullptr, false },                                                 // LeftLargeZeroGRollUp
        { nullptr, nullptr, nullptr, false },                                                 // RightLargeZeroGRollUp
        { nullptr, nullptr, nullptr, true },                                                  // LeftLargeZeroGRollDown
        { nullptr, nullptr, nullptr, true },                                                  // RightLargeZeroGRollDown
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
        { nullptr, nullptr, nullptr, true },  // LeftEighthToDiagDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthToDiagDown25
        { nullptr, nullptr, nullptr, false }, // LeftEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthToOrthogonalUp25
        { nullptr, nullptr, nullptr, true },  // LeftEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthToOrthogonalDown25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagUp25ToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToUp25
        { nullptr, nullptr, nullptr, true },  // DiagDown25ToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagDown25ToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedDown25ToDown25
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedDown25ToDown25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToRightBankedFlat
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedDown25ToLeftBankedFlat
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedDown25ToRightBankedFlat
        { nullptr, nullptr, nullptr, false }, // DiagFlatToLeftBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagFlatToRightBankedUp25
        { nullptr, nullptr, nullptr, false }, // DiagLeftBankedUp25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagRightBankedUp25ToFlat
        { nullptr, nullptr, nullptr, true },  // DiagFlatToLeftBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagFlatToRightBankedDown25
        { nullptr, nullptr, nullptr, true },  // DiagLeftBankedDown25ToFlat
        { nullptr, nullptr, nullptr, true },  // DiagRightBankedDown25ToFlat
        { nullptr, nullptr, nullptr, false }, // DiagUp25LeftBanked
        { nullptr, nullptr, nullptr, false }, // DiagUp25RightBanked
        { nullptr, nullptr, nullptr, true },  // DiagDown25LeftBanked
        { nullptr, nullptr, nullptr, true },  // DiagDown25RightBanked
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToDiagUp25
        { nullptr, nullptr, nullptr, true },  // LeftEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthBankToDiagDown25
        { nullptr, nullptr, nullptr, false }, // LeftEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, false }, // RightEighthBankToOrthogonalUp25
        { nullptr, nullptr, nullptr, true },  // LeftEighthBankToOrthogonalDown25
        { nullptr, nullptr, nullptr, true },  // RightEighthBankToOrthogonalDown25
        { &OpenRCT2::WoodenRollerCoaster::kDiagBrakesSprites[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagBrakesOffsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagBrakesBoundBoxes[0][0][0], false }, // DiagBrakes
        { &OpenRCT2::WoodenRollerCoaster::kDiagBlockBrakesSprites[0][0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagBlockBrakesOffsets[0][0][0],
          &OpenRCT2::WoodenRollerCoaster::kDiagBlockBrakesBoundBoxes[0][0][0], false }, // DiagBlockBrakes
        { nullptr, nullptr, nullptr, false },                                           // Down25Brakes
        { nullptr, nullptr, nullptr, false },                                           // DiagBooster
        { nullptr, nullptr, nullptr, false },                                           // DiagFlatToUp60LongBase
        { nullptr, nullptr, nullptr, false },                                           // DiagUp60ToFlatLongBase
        { nullptr, nullptr, nullptr, true },                                            // DiagFlatToDown60LongBase
        { nullptr, nullptr, nullptr, true },                                            // DiagDown60ToFlatLongBase
        { nullptr, nullptr, nullptr, false },                                           // LeftEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, false },                                           // RightEighthDiveLoopUpToOrthogonal
        { nullptr, nullptr, nullptr, true },                                            // LeftEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, true },                                            // RightEighthDiveLoopDownToDiag
        { nullptr, nullptr, nullptr, false },                                           // DiagDown25Brakes
    } };
} // namespace OpenRCT2::ClassicWoodenTwisterRollerCoaster
