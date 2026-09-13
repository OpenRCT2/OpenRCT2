/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../actions/GameCommand.h"
#include "../core/Money.hpp"
#include "../world/Location.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

namespace OpenRCT2
{
    struct Peep;
}

namespace OpenRCT2::Network
{
    struct Packet;

    class Player final
    {
    public:
        uint8_t id = 0;
        std::string name;
        uint16_t ping = 0;
        uint8_t flags = 0;
        uint8_t group = 0;
        money64 moneySpent = 0.00_GBP;
        uint32_t commandsRan = 0;
        int32_t lastAction = -999;
        uint32_t lastActionTime = 0;
        CoordsXYZ lastActionCoord = {};
        Peep* pickupPeep = nullptr;
        int32_t pickupPeepOldX = kLocationNull;
        std::string keyHash;
        uint32_t lastDemolishRideTime = 0;
        uint32_t lastPlaceSceneryTime = 0;
        std::unordered_map<GameCommand, int32_t> cooldownTime;
        Player() noexcept = default;

        void setName(std::string_view name);

        void read(Packet& packet);
        void write(Packet& packet);
        void incrementNumCommands();
        void addMoneySpent(money64 cost);
    };
} // namespace OpenRCT2::Network
