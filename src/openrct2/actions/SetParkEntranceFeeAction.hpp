/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Cheats.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../world/Park.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(SetParkEntranceFeeAction, GAME_COMMAND_SET_PARK_ENTRANCE_FEE, GameActions::Result)
{
private:
    money16 _fee{ MONEY16_UNDEFINED };

public:
    SetParkEntranceFeeAction() = default;
    SetParkEntranceFeeAction(money16 fee)
        : _fee(fee)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_fee);
    }

    GameActions::Result::Ptr Query() const override
    {
        bool noMoney = (gParkFlags & PARK_FLAGS_NO_MONEY) != 0;
        bool forceFreeEntry = !park_entry_price_unlocked();
        if (noMoney || forceFreeEntry)
        {
            return std::make_unique<GameActions::Result>(GameActions::Status::Disallowed, STR_NONE);
        }
        if (_fee < MONEY_FREE || _fee > MAX_ENTRANCE_FEE)
        {
            return std::make_unique<GameActions::Result>(GameActions::Status::InvalidParameters, STR_NONE);
        }
        return std::make_unique<GameActions::Result>();
    }

    GameActions::Result::Ptr Execute() const override
    {
        gParkEntranceFee = _fee;
        window_invalidate_by_class(WC_PARK_INFORMATION);
        return std::make_unique<GameActions::Result>();
    }
};
