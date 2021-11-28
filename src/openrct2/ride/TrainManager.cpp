/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrainManager.h"

#include "../entity/EntityList.h"
#include "../entity/EntityRegistry.h"
#include "Vehicle.h"

namespace TrainManager
{
    View::Iterator& View::Iterator::operator++()
    {
        Entity = nullptr;

        while (index >= 0 && Entity == nullptr)
        {
            if (index >= vec->size())
            {
                // This can happen if multiple entities were removed at once.
                continue;
            }
            Entity = GetEntity<Vehicle>((*vec)[index--]);
            if (Entity != nullptr && !Entity->IsHead())
            {
                Entity = nullptr;
            }
        }
        return *this;
    }

    View::View()
    {
        vec = &GetEntityList(EntityType::Vehicle);
    }
} // namespace TrainManager
