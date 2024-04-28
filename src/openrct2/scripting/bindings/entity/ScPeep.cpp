/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScPeep.hpp"

#    include "../../../peep/PeepAnimationData.h"

namespace OpenRCT2::Scripting
{
    // TODO: move other method implementations from ScPeep.hpp here.

    std::vector<uint32_t> ScPeep::getWalkingSpriteIds() const
    {
        std::vector<uint32_t> spriteIds{};
        auto peep = GetPeep();
        if (peep != nullptr)
        {
            auto& animationGroup = GetPeepAnimation(peep->SpriteType, PeepActionSpriteType::None);
            for (auto frameOffset : animationGroup.frame_offsets)
            {
                constexpr auto rotation = 1;
                spriteIds.push_back(animationGroup.base_image + rotation + frameOffset * 4);
            }
        }
        return spriteIds;
    }
} // namespace OpenRCT2::Scripting

#endif
