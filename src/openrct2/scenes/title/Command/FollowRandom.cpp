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
#include "../../../ride/Vehicle.h"
#include "../../../util/Util.h"

namespace OpenRCT2::Title
{
    int16_t FollowRandomCommand::operator()(int16_t timer)
    {
        auto* w = WindowGetMain();
        if (w != nullptr)
        {
            const auto& entities = getGameState().entities.GetEntityList(Follow.Type);
            if (Follow.Type != EntityType::null && !entities.empty())
            {
                auto it = entities.begin();
                if (Follow.Type == EntityType::vehicle)
                {
                    // Do 5 rolls to make it more likely that a vehicle that's currently travelling is chosen
                    for (int roll = 0; roll < 5; roll++, it = entities.begin())
                    {
                        std::advance(it, UtilRand() % entities.size());
                        Vehicle* entity = getGameState().entities.GetEntity(*it)->As<Vehicle>();
                        if (entity && entity->status == Vehicle::Status::Travelling)
                            break;
                    }
                }
                else
                {
                    std::advance(it, UtilRand() % entities.size());
                }

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
