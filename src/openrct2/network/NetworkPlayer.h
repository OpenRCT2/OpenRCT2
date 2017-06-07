#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include <string>
#include "../common.h"

extern "C"
{
    #include "../peep/peep.h"
    #include "../world/map.h"
    #include "../world/sprite.h"
}

class NetworkPacket;

class NetworkPlayer final
{
public:
    uint8       Id                      = 0;
    std::string Name;
    uint16      Ping                    = 0;
    uint8       Flags                   = 0;
    uint8       Group                   = 0;
    money32     MoneySpent              = MONEY(0, 0);
    uint32      CommandsRan             = 0;
    sint32      LastAction              = -999;
    uint32      LastActionTime          = 0;
    rct_xyz16   LastActionCoord         = { 0 };
    rct_peep*   PickupPeep              = 0;
    sint32      PickupPeepOldX          = SPRITE_LOCATION_NULL;
    std::string KeyHash;
    uint32      LastDemolishRideTime    = 0;
    uint32      LastPlaceSceneryTime    = 0;

    NetworkPlayer() = default;

    void SetName(const std::string &name);

    void Read(NetworkPacket &packet);
    void Write(NetworkPacket &packet);
    void AddMoneySpent(money32 cost);
};
