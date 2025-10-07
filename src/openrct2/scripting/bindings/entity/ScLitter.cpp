/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScLitter.hpp"

    #include "../../../core/EnumMap.hpp"
    #include "../../../entity/Litter.h"

namespace OpenRCT2::Scripting
{
    static const EnumMap<Litter::Type> LitterTypeMap(
        {
            { "vomit", Litter::Type::vomit },
            { "vomit_alt", Litter::Type::vomitAlt },
            { "empty_can", Litter::Type::emptyCan },
            { "rubbish", Litter::Type::rubbish },
            { "burger_box", Litter::Type::burgerBox },
            { "empty_cup", Litter::Type::emptyCup },
            { "empty_box", Litter::Type::emptyBox },
            { "empty_bottle", Litter::Type::emptyBottle },
            { "empty_bowl_red", Litter::Type::emptyBowlRed },
            { "empty_drink_carton", Litter::Type::emptyDrinkCarton },
            { "empty_juice_cup", Litter::Type::emptyJuiceCup },
            { "empty_bowl_blue", Litter::Type::emptyBowlBlue },
        });

    JSValue ScLitter::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = gScEntity.New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScLitter::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("litterType", &ScLitter::litterType_get, &ScLitter::litterType_set),
            JS_CGETSET_DEF("creationTick", &ScLitter::creationTick_get, nullptr)
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    Litter* ScLitter::GetLitter(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<Litter>(id);
    }

    JSValue ScLitter::litterType_get(JSContext* ctx, JSValue thisVal)
    {
        auto* litter = GetLitter(thisVal);
        if (litter != nullptr)
        {
            auto it = LitterTypeMap.find(litter->SubType);
            if (it != LitterTypeMap.end())
            {
                return JSFromStdString(ctx, it->first);
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScLitter::litterType_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(litterType, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto it = LitterTypeMap.find(litterType);
        if (it != LitterTypeMap.end())
        {
            auto* litter = GetLitter(thisVal);
            litter->SubType = it->second;
            litter->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScLitter::creationTick_get(JSContext* ctx, JSValue thisVal)
    {
        auto* litter = GetLitter(thisVal);
        return JS_NewUint32(ctx, litter == nullptr ? 0 : litter->creationTick);
    }
} // namespace OpenRCT2::Scripting

#endif
