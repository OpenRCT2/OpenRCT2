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

#include "../core/MemoryStream.h"
#include "../localisation/string_ids.h"
#include "GameAction.h"

extern "C"
{
    #include "../cheats.h"
    #include "../interface/window.h"
    #include "../world/park.h"
}

struct SetParkEntranceFeeAction : public GameAction<GAME_COMMAND_SET_PARK_ENTRANCE_FEE, GA_FLAGS::ALLOW_WHILE_PAUSED>
{
public:
    money16 Fee;

    void Deserialise(IStream * stream) override
    {
        Fee = stream->ReadValue<money16>();
    }

    void Serialise(IStream * stream) const override
    {
        stream->WriteValue(Fee);
    }

    GameActionResult Query(uint32 flags = 0) const override
    {
        bool noMoney = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
        bool forceFreeEntry = (gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && !gCheatsUnlockAllPrices;
        if (noMoney || forceFreeEntry)
        {
            return GameActionResult(GA_ERROR::DISALLOWED, STR_NONE);
        }
        if (Fee < MONEY_FREE || Fee > MONEY(100,00))
        {
            return GameActionResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return GameActionResult();
    }

    GameActionResult Execute(uint32 flags = 0) const override
    {
        gParkEntranceFee = Fee;
        window_invalidate_by_class(WC_PARK_INFORMATION);
        return GameActionResult();
    }
};

static auto Factory UNUSED_ATTR = GameActions::Register<SetParkEntranceFeeAction>();

extern "C"
{
    void park_set_entrance_fee(money32 value)
    {
        auto gameAction = SetParkEntranceFeeAction();
        gameAction.Fee = (money16)value;
        GameActions::Execute(&gameAction);
    }

    void game_command_set_park_entrance_fee(int *eax, int *ebx, int *ecx, int *edx, int *esi, int *edi, int *ebp)
    {
        auto gameAction = SetParkEntranceFeeAction();
        gameAction.Fee = (*edi & 0xFFFF);
        GameActions::Execute(&gameAction);
    }
}
