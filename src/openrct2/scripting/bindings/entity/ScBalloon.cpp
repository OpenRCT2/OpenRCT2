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
    ScBalloon gScBalloon;

    JSValue ScBalloon::New(JSContext* ctx, EntityId entityId)
    {
        return gScEntity.NewDerivedInstance(ctx, entityId, gScBalloon.GetProto());
    }

    void ScBalloon::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = { JS_CGETSET_DEF(
            "colour", &ScBalloon::colour_get, &ScBalloon::colour_set) };
        gScBalloon.RegisterDerived(ctx, gScEntity, funcs);
    }

    Balloon* ScBalloon::GetBalloon(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<Balloon>(id);
    }

    JSValue ScBalloon::colour_get(JSContext* ctx, JSValue thisVal)
    {
        auto balloon = GetBalloon(thisVal);
        return JS_NewUint32(ctx, balloon == nullptr ? 0 : EnumValue(balloon->colour));
    }

    JSValue ScBalloon::colour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto balloon = GetBalloon(thisVal);
        if (balloon != nullptr)
        {
            balloon->colour = static_cast<Drawing::Colour>(value);
            balloon->Invalidate();
        }
        return JS_UNDEFINED;
    }

} // namespace OpenRCT2::Scripting

#endif
