/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "GameAction.h"

DEFINE_GAME_ACTION(CustomAction, GAME_COMMAND_CUSTOM, GameActions::Result)
{
private:
    std::string _id;
    std::string _json;

public:
    CustomAction() = default;
    CustomAction(const std::string& id, const std::string& json)
        : _id(id)
        , _json(json)
    {
    }

    std::string GetId() const
    {
        return _id;
    }

    std::string GetJson() const
    {
        return _json;
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
    }

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};

#endif
