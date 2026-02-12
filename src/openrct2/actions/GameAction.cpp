/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "GameAction.hpp"

#include "../Context.h"
#include "../core/Guard.hpp"
#include "../scripting/Duktape.hpp"
#include "../scripting/HookEngine.h"
#include "../scripting/ScriptEngine.h"
#include "../world/Map.h"
#include "GameActionRegistry.h"
#include "GameActionRunner.h"

#include <iterator>

namespace OpenRCT2::GameActions
{
    const char* GameAction::GetName() const
    {
        return GameActions::GetName(_type);
    }

    bool GameAction::LocationValid(const CoordsXY& coords) const
    {
        auto result = MapIsLocationValid(coords);
        if (!result)
            return false;
#ifdef ENABLE_SCRIPTING
        auto& hookEngine = GetContext()->GetScriptEngine().GetHookEngine();
        if (hookEngine.HasSubscriptions(Scripting::HookType::actionLocation))
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();

            // Create event args object
            auto obj = Scripting::DukObject(ctx);
            obj.Set("x", coords.x);
            obj.Set("y", coords.y);
            obj.Set("player", _playerId);
            obj.Set("type", EnumValue(_type));

            auto flags = GetActionFlags();
            obj.Set("isClientOnly", (flags & Flags::ClientOnly) != 0);
            obj.Set("result", true);

            // Call the subscriptions
            auto e = obj.Take();
            hookEngine.Call(Scripting::HookType::actionLocation, e, true);

            auto scriptResult = Scripting::AsOrDefault(e["result"], true);

            return scriptResult;
        }
#endif
        return true;
    }

    GameAction::Ptr Create(GameCommand id)
    {
        GameAction* result = nullptr;

        auto factory = getFactory(id);
        if (factory.has_value())
        {
            result = (*factory)();
        }

        Guard::ArgumentNotNull(result, "Attempting to create unregistered game action: %u", id);
        return std::unique_ptr<GameAction>(result);
    }

    GameAction::Ptr Clone(const GameAction* action)
    {
        std::unique_ptr<GameAction> ga = Create(action->GetType());
        ga->SetCallback(action->GetCallback());

        // Serialise action data into stream.
        DataSerialiser dsOut(true);
        action->Serialise(dsOut);

        // Serialise into new action.
        IStream& stream = dsOut.GetStream();
        stream.SetPosition(0);

        DataSerialiser dsIn(false, stream);
        ga->Serialise(dsIn);

        return ga;
    }

} // namespace OpenRCT2::GameActions
