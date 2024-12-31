/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING
    #include "CustomAction.h"

    #include "../Context.h"
    #include "../scripting/ScriptEngine.h"

using namespace OpenRCT2;

CustomAction::CustomAction(const std::string& id, const std::string& json, const std::string& pluginName)
    : _id(id)
    , _json(json)
    , _pluginName(pluginName)
{
}

std::string CustomAction::GetId() const
{
    return _id;
}

std::string CustomAction::GetJson() const
{
    return _json;
}

std::string CustomAction::GetPluginName() const
{
    return _pluginName;
}

uint16_t CustomAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void CustomAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_id) << DS_TAG(_json);
}

GameActions::Result CustomAction::Query() const
{
    auto& scriptingEngine = OpenRCT2::GetContext()->GetScriptEngine();
    return scriptingEngine.QueryOrExecuteCustomGameAction(*this, false);
}

GameActions::Result CustomAction::Execute() const
{
    auto& scriptingEngine = OpenRCT2::GetContext()->GetScriptEngine();
    return scriptingEngine.QueryOrExecuteCustomGameAction(*this, true);
}

#endif
