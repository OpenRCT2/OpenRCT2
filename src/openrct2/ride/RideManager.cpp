/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    RideManager::RideManager()
        : _gameState(getGameState()) {};

    RideManager::RideManager(GameState_t& gameState)
        : _gameState(gameState) {};

    size_t RideManager::size() const
    {
        size_t count = 0;
        for (size_t i = 0; i < _gameState.ridesEndOfUsedRange; i++)
        {
            if (!_gameState.rides[i].id.IsNull())
            {
                count++;
            }
        }
        return count;
    }

    RideManager::Iterator RideManager::begin()
    {
        return RideManager::Iterator(*this, 0u, _gameState.ridesEndOfUsedRange);
    }

    RideManager::Iterator RideManager::end()
    {
        return RideManager::Iterator(*this, _gameState.ridesEndOfUsedRange, _gameState.ridesEndOfUsedRange);
    }

    RideManager::Iterator RideManager::get(RideId rideId)
    {
        return RideManager::Iterator(*this, rideId.ToUnderlying(), _gameState.ridesEndOfUsedRange);
    }

    RideManager GetRideManager()
    {
        return {};
    }
} // namespace OpenRCT2
