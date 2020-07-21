/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../GameState.h"
#include "../config/Config.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/Localisation.h"
#include "../management/Finance.h"
#include "../network/network.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(ParkSetNameAction, GAME_COMMAND_SET_PARK_NAME, GameActionResult)
{
private:
    std::string _name;

public:
    ParkSetNameAction()
    {
    }
    ParkSetNameAction(const std::string& name)
        : _name(name)
    {
    }

    void AcceptParameters(GameActionParameterVisitor & visitor) override
    {
        visitor.Visit("name", _name);
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_name);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_name.empty())
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_RENAME_PARK, STR_INVALID_NAME_FOR_PARK);
        }
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        // Do a no-op if new name is the same as the current name is the same
        auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
        if (_name != park.Name)
        {
            park.Name = _name;
            scrolling_text_invalidate();
            gfx_invalidate_screen();
        }
        return MakeResult();
    }
};
