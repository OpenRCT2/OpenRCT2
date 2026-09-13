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
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, coords.x));
            JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, coords.y));
            JS_SetPropertyStr(ctx, obj, "player", JS_NewInt32(ctx, _playerId));
            JS_SetPropertyStr(ctx, obj, "type", JS_NewInt32(ctx, EnumValue(_type)));

            auto flags = GetActionFlags();
            JS_SetPropertyStr(ctx, obj, "isClientOnly", JS_NewBool(ctx, (flags & Flags::ClientOnly) != 0));
            JS_SetPropertyStr(ctx, obj, "result", JS_NewBool(ctx, true));

            // Call the subscriptions
            hookEngine.Call(Scripting::HookType::actionLocation, obj, true, true);

            auto scriptResult = Scripting::AsOrDefault(ctx, obj, "result", true);
            JS_FreeValue(ctx, obj);

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
