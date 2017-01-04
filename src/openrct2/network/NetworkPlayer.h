#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
    uint8       id                  = 0;
    std::string name;
    uint16      ping                = 0;
    uint8       flags               = 0;
    uint8       group               = 0;
    money32     money_spent         = MONEY(0, 0);
    uint32      commands_ran        = 0;
    int         last_action         = -999;
    uint32      last_action_time    = 0;
    rct_xyz16   last_action_coord   = { 0 };
    rct_peep*   pickup_peep         = 0;
    int         pickup_peep_old_x   = SPRITE_LOCATION_NULL;
    std::string keyhash;

    NetworkPlayer() = default;

    void SetName(const std::string &name);

    void Read(NetworkPacket &packet);
    void Write(NetworkPacket &packet);
    void AddMoneySpent(money32 cost);
};
