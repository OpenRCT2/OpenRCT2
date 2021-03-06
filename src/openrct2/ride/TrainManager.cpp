/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrainManager.h"

#include "../world/Entity.h"
#include "../world/EntityList.h"
#include "Vehicle.h"

namespace TrainManager
{
    TrainView::TrainViewIterator& TrainView::TrainViewIterator::operator++()
    {
        Entity = nullptr;

        while (iter != end && Entity == nullptr)
        {
            Entity = GetEntity<Vehicle>(*iter++);
            if (Entity && !Entity->IsHead())
            {
                Entity = nullptr;
            }
        }
        return *this;
    }

    TrainView::TrainView()
    {
        vec = &GetEntityList(EntityListId::Vehicle);
    }
} // namespace TrainManager
