/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING
#    include "CustomAction.h"

#    include "../Context.h"
#    include "../scripting/ScriptEngine.h"

void CustomAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_id) << DS_TAG(_json);
}

GameActions::Result::Ptr CustomAction::Query() const
{
    auto& scriptingEngine = OpenRCT2::GetContext()->GetScriptEngine();
    return scriptingEngine.QueryOrExecuteCustomGameAction(_id, _json, false);
}

GameActions::Result::Ptr CustomAction::Execute() const
{
    auto& scriptingEngine = OpenRCT2::GetContext()->GetScriptEngine();
    return scriptingEngine.QueryOrExecuteCustomGameAction(_id, _json, true);
}

#endif
