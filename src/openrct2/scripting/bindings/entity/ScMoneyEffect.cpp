/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScMoneyEffect.hpp"

    #include "../../../GameState.h"
    #include "../../../entity/MoneyEffect.h"

namespace OpenRCT2::Scripting
{
    JSValue ScMoneyEffect::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = gScEntity.New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScMoneyEffect::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = { JS_CGETSET_DEF(
            "value", &ScMoneyEffect::value_get, &ScMoneyEffect::value_set) };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    MoneyEffect* ScMoneyEffect::GetMoneyEffect(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<MoneyEffect>(id);
    }

    JSValue ScMoneyEffect::value_get(JSContext* ctx, JSValue thisVal)
    {
        auto moneyEffect = GetMoneyEffect(thisVal);
        return JS_NewUint32(ctx, moneyEffect == nullptr ? 0 : moneyEffect->Value);
    }

    JSValue ScMoneyEffect::value_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_MONEY64(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto moneyEffect = GetMoneyEffect(thisVal);
        if (moneyEffect != nullptr)
        {
            moneyEffect->SetValue(value);
        }
        return JS_UNDEFINED;
    }
} // namespace OpenRCT2::Scripting

#endif
