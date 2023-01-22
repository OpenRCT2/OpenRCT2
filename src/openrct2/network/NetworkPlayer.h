/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Game.h"
#include "../common.h"
#include "../world/Map.h"

#include <string>
#include <string_view>
#include <unordered_map>
namespace OpenRCT2
{
    struct NetworkPacket;
    struct Peep;

    class NetworkPlayer final
    {
    public:
        uint8_t Id = 0;
        std::string Name;
        uint16_t Ping = 0;
        uint8_t Flags = 0;
        uint8_t Group = 0;
        money32 MoneySpent = 0.00_GBP;
        uint32_t CommandsRan = 0;
        int32_t LastAction = -999;
        uint32_t LastActionTime = 0;
        CoordsXYZ LastActionCoord = {};
        Peep* PickupPeep = nullptr;
        int32_t PickupPeepOldX = LOCATION_NULL;
        std::string KeyHash;
        uint32_t LastDemolishRideTime = 0;
        uint32_t LastPlaceSceneryTime = 0;
        std::unordered_map<GameCommand, int32_t> CooldownTime;
        NetworkPlayer() noexcept = default;

        void SetName(std::string_view name);

        void Read(NetworkPacket& packet);
        void Write(NetworkPacket& packet);
        void AddMoneySpent(money32 cost);
    };
} // namespace OpenRCT2
