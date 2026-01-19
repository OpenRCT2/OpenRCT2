/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "../core/Money.hpp"
#include "../world/Location.hpp"

#include <string>
#include <string_view>
#include <unordered_map>

struct Peep;

namespace OpenRCT2::Network
{
    struct Packet;

    class Player final
    {
    public:
        uint8_t Id = 0;
        std::string Name;
        uint16_t Ping = 0;
        uint8_t Flags = 0;
        uint8_t Group = 0;
        money64 MoneySpent = 0.00_GBP;
        uint32_t CommandsRan = 0;
        int32_t LastAction = -999;
        uint32_t LastActionTime = 0;
        CoordsXYZ LastActionCoord = {};
        Peep* PickupPeep = nullptr;
        int32_t PickupPeepOldX = kLocationNull;
        std::string KeyHash;
        uint32_t LastDemolishRideTime = 0;
        uint32_t LastPlaceSceneryTime = 0;
        std::unordered_map<GameCommand, int32_t> CooldownTime;
        Player() noexcept = default;

        void SetName(std::string_view name);

        void Read(Packet& packet);
        void Write(Packet& packet);
        void IncrementNumCommands();
        void AddMoneySpent(money64 cost);
    };
} // namespace OpenRCT2::Network
