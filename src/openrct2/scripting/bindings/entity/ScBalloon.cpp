/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScBalloon.hpp"

    #include "../../../entity/Balloon.h"

namespace OpenRCT2::Scripting
{
    JSValue ScBalloon::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = gScEntity.New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScBalloon::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = { JS_CGETSET_DEF(
            "colour", &ScBalloon::colour_get, &ScBalloon::colour_set) };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    Balloon* ScBalloon::GetBalloon(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<Balloon>(id);
    }

    JSValue ScBalloon::colour_get(JSContext* ctx, JSValue thisVal)
    {
        auto balloon = GetBalloon(thisVal);
        return JS_NewUint32(ctx, balloon == nullptr ? 0 : balloon->colour);
    }

    JSValue ScBalloon::colour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto balloon = GetBalloon(thisVal);
        if (balloon != nullptr)
        {
            balloon->colour = value;
            balloon->Invalidate();
        }
        return JS_UNDEFINED;
    }

} // namespace OpenRCT2::Scripting

#endif
