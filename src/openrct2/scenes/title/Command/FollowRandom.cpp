/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FollowRandom.h"

#include "../../../entity/EntityList.h"
#include "../../../interface/WindowBase.h"
#include "../../../util/Util.h"

namespace OpenRCT2::Title
{
    int16_t FollowRandomCommand::operator()(int16_t timer)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            if (Follow.Type != EntityType::null)
            {
                const auto& entities = getGameState().entities.GetEntityList(Follow.Type);
                auto it = entities.begin();
                std::advance(it, UtilRand() % entities.size());
                if (Follow.ScrollToLocation)
                    w->flags.set(WindowFlag::scrollingToLocation);
                WindowFollowSprite(*w, *it);
            }
            else
            {
                WindowFollowSprite(*w, EntityId::GetNull());
            }
        }

        return 0;
    }
} // namespace OpenRCT2::Title
