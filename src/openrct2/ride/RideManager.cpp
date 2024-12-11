/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideManager.hpp"

#include "../GameState.h"

namespace OpenRCT2
{
    size_t RideManager::size() const
    {
        auto& gameState = GetGameState();
        size_t count = 0;
        for (size_t i = 0; i < _endOfUsedRange; i++)
        {
            if (!gameState.Rides[i].id.IsNull())
            {
                count++;
            }
        }
        return count;
    }

    RideManager::Iterator RideManager::begin()
    {
        return RideManager::Iterator(*this, 0u, _endOfUsedRange);
    }

    RideManager::Iterator RideManager::end()
    {
        return RideManager::Iterator(*this, _endOfUsedRange, _endOfUsedRange);
    }

    RideManager::Iterator RideManager::get(RideId rideId)
    {
        return RideManager::Iterator(*this, rideId.ToUnderlying(), _endOfUsedRange);
    }

    RideManager GetRideManager()
    {
        return {};
    }
} // namespace OpenRCT2
