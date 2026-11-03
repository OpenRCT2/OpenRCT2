/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../paint/track/TrackPaintGeneric.h"

namespace OpenRCT2
{
    static constexpr uint8_t kQuarterHelixSequenceCount = 7;
    static constexpr uint8_t kQuarterHelixSequenceSpriteCount = 2;

    static constexpr bool kLeftQuarterHelixLargeUpSpriteMapArray[kNumOrthogonalDirections][kQuarterHelixSequenceCount]
                                                                [kQuarterHelixSequenceSpriteCount]
        = { { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } } };

    static constexpr bool kLeftQuarterBankedHelixLargeUpSpriteMapArray[kNumOrthogonalDirections][kQuarterHelixSequenceCount]
                                                                      [kQuarterHelixSequenceSpriteCount]
        = { { { true, true },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } },
            { { false, true },
              { false, false },
              { false, true },
              { false, false },
              { false, false },
              { false, true },
              { false, true } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { false, true } },
            { { true, false },
              { false, false },
              { true, false },
              { true, false },
              { false, false },
              { true, false },
              { true, false } } };

    static constexpr uint64_t kLeftQuarterHelixLargeUpSpriteMap = createSpriteMap<false>(
        kLeftQuarterHelixLargeUpSpriteMapArray);
    static constexpr uint64_t kRightQuarterHelixLargeUpSpriteMap = createSpriteMap<true>(
        kLeftQuarterHelixLargeUpSpriteMapArray);
    static constexpr uint64_t kLeftQuarterBankedHelixLargeUpSpriteMap = createSpriteMap<false>(
        kLeftQuarterBankedHelixLargeUpSpriteMapArray);
    static constexpr uint64_t kRightQuarterBankedHelixLargeUpSpriteMap = createSpriteMap<true>(
        kLeftQuarterBankedHelixLargeUpSpriteMapArray);

    typedef std::array<
        std::array<std::array<BoundBoxXYZ, kQuarterHelixSequenceSpriteCount>, kQuarterHelixSequenceCount>,
        kNumOrthogonalDirections>
        QuarterHelixBoundingBoxes;
    extern const QuarterHelixBoundingBoxes kLeftQuarterHelixLargeUpBoundingBoxes;
    extern const QuarterHelixBoundingBoxes kRightQuarterHelixLargeUpBoundingBoxes;
} // namespace OpenRCT2
