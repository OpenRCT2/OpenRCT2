/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "GameAction.h"

#include "../Cheats.h"
#include "../interface/Window.h"
#include "../world/Park.h"

struct SetParkEntranceFeeAction : public GameActionBase<GAME_COMMAND_SET_PARK_ENTRANCE_FEE, GameActionResult>
{
private:
    money16 _fee;

public:
    SetParkEntranceFeeAction() {}
    SetParkEntranceFeeAction(money16 fee) 
        : _fee(fee)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser& stream) override
    {
        GameAction::Serialise(stream);

        stream << _fee;
    }

    GameActionResult::Ptr Query() const override
    {
        bool noMoney = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
        bool forceFreeEntry = !park_entry_price_unlocked();
        if (noMoney || forceFreeEntry)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::DISALLOWED, STR_NONE);
        }
        if (_fee < MONEY_FREE || _fee > MAX_ENTRANCE_FEE)
        {
            return std::make_unique<GameActionResult>(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }
        return std::make_unique<GameActionResult>();
    }

    GameActionResult::Ptr Execute() const override
    {
        gParkEntranceFee = _fee;
        window_invalidate_by_class(WC_PARK_INFORMATION);
        return std::make_unique<GameActionResult>();
    }
};

