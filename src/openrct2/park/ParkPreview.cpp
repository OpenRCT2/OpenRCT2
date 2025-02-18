/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkPreview.h"

#include "../Context.h"
#include "../GameState.h"
#include "../ride/RideManager.hpp"

namespace OpenRCT2
{
    ParkPreview generatePreviewFromGameState(const GameState_t& gameState)
    {
        ParkPreview preview{
            .parkName = gameState.Park.Name,
            .parkRating = gameState.Park.Rating,
            .year = gameState.Date.GetYear(),
            .month = gameState.Date.GetMonth(),
            .day = gameState.Date.GetDay(),
            .parkUsesMoney = !(gameState.Park.Flags & PARK_FLAGS_NO_MONEY),
            .cash = gameState.Cash,
            .numRides = static_cast<uint16_t>(RideManager().size()),
            .numGuests = static_cast<uint16_t>(gameState.NumGuestsInPark),
        };

        return preview;
    }
} // namespace OpenRCT2
