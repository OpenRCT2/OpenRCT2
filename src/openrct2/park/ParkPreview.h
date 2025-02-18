/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"

#include <cstdint>
#include <string>

namespace OpenRCT2
{
    struct ParkPreview
    {
        std::string parkName{};
        uint16_t parkRating{};
        int32_t year{};
        int32_t month{};
        int32_t day{};
        bool parkUsesMoney{ true };
        money64 cash{};
        uint16_t numRides{};
        uint16_t numGuests{};
    };

    struct GameState_t;

    ParkPreview generatePreviewFromGameState(const GameState_t& gameState);
} // namespace OpenRCT2
