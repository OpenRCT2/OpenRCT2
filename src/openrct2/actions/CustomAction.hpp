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

#    include "../Context.h"
#    include "../scripting/ScriptEngine.h"
#    include "GameAction.h"

DEFINE_GAME_ACTION(CustomAction, GAME_COMMAND_CUSTOM, GameActionResult)
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
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_id) << DS_TAG(_json);
    }

    GameActionResult::Ptr Query() const override
    {
        auto& scriptingEngine = OpenRCT2::GetContext()->GetScriptEngine();
        return scriptingEngine.QueryOrExecuteCustomGameAction(_id, _json, false);
    }

    GameActionResult::Ptr Execute() const override
    {
        auto& scriptingEngine = OpenRCT2::GetContext()->GetScriptEngine();
        return scriptingEngine.QueryOrExecuteCustomGameAction(_id, _json, true);
    }
};

#endif
