#ifdef ENABLE_SCRIPTING

    #include "ScDuck.hpp"

    #include "../../../GameState.h"
    #include "../../../entity/Duck.h"

namespace OpenRCT2::Scripting
{
    ScDuck gScDuck;

    namespace
    {
        constexpr size_t GetAnimationLength(OpenRCT2::Duck::DuckState state)
        {
            switch (state)
            {
                case OpenRCT2::Duck::DuckState::flyToWater:
                    return 6;
                case OpenRCT2::Duck::DuckState::swim:
                    return 1;
                case OpenRCT2::Duck::DuckState::drink:
                    return 24;
                case OpenRCT2::Duck::DuckState::doubleDrink:
                    return 39;
                case OpenRCT2::Duck::DuckState::flyAway:
                    return 6;
            }
            return 0;
        }

        void AdvanceFrozenAnimation(OpenRCT2::Duck* duck)
        {
            if ((getGameState().currentTicks & 3) != 0)
            {
                return;
            }

            const auto length = GetAnimationLength(duck->getState());
            if (length == 0)
            {
                return;
            }

            duck->frame++;
            duck->frame %= static_cast<uint16_t>(length);
            duck->invalidate();
        }

        constexpr const char* kDuckAnimationNames[] = {
            "flyToWater",
            "swim",
            "drink",
            "doubleDrink",
            "flyAway",
        };

        std::optional<OpenRCT2::Duck::DuckState> GetDuckStateFromName(std::string_view name)
        {
            for (size_t i = 0; i < std::size(kDuckAnimationNames); i++)
            {
                if (name == kDuckAnimationNames[i])
                {
                    return static_cast<OpenRCT2::Duck::DuckState>(i);
                }
            }
            return std::nullopt;
        }
    }

    JSValue ScDuck::New(JSContext* ctx, EntityId entityId)
    {
        return gScEntity.NewDerivedInstance(ctx, entityId, gScDuck.GetProto());
    }

    void ScDuck::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("positionFrozen", &ScDuck::positionFrozen_get, &ScDuck::positionFrozen_set),
            JS_CGETSET_DEF("animationFrozen", &ScDuck::animationFrozen_get, &ScDuck::animationFrozen_set),
            JS_CGETSET_DEF("orientation", &ScDuck::orientation_get, &ScDuck::orientation_set),
            JS_CGETSET_DEF("animationFrame", &ScDuck::animationFrame_get, &ScDuck::animationFrame_set),
            JS_CGETSET_DEF("animation", &ScDuck::animation_get, &ScDuck::animation_set),
        };
        gScDuck.RegisterDerived(ctx, gScEntity, funcs);
    }

    Duck* ScDuck::GetDuck(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.getEntity<Duck>(id);
    }

    JSValue ScDuck::positionFrozen_get(JSContext* ctx, JSValue thisVal)
    {
        auto duck = GetDuck(thisVal);
        return JS_NewBool(ctx, duck != nullptr && duck->getPositionFrozen());
    }

    JSValue ScDuck::positionFrozen_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto duck = GetDuck(thisVal);
        if (duck != nullptr)
        {
            duck->setPositionFrozen(value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScDuck::animationFrozen_get(JSContext* ctx, JSValue thisVal)
    {
        auto duck = GetDuck(thisVal);
        return JS_NewBool(ctx, duck != nullptr && duck->getAnimationFrozen());
    }

    JSValue ScDuck::animationFrozen_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_BOOL(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto duck = GetDuck(thisVal);
        if (duck != nullptr)
        {
            duck->setAnimationFrozen(value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScDuck::orientation_get(JSContext* ctx, JSValue thisVal)
    {
        auto duck = GetDuck(thisVal);
        return JS_NewUint32(ctx, duck != nullptr ? duck->orientation : 0);
    }

    JSValue ScDuck::orientation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto duck = GetDuck(thisVal);
        if (duck != nullptr)
        {
            duck->orientation = static_cast<uint8_t>(std::min<uint32_t>(value, 31));
            duck->invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScDuck::animationFrame_get(JSContext* ctx, JSValue thisVal)
    {
        auto duck = GetDuck(thisVal);
        return JS_NewUint32(ctx, duck != nullptr ? duck->frame : 0);
    }

    JSValue ScDuck::animationFrame_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto duck = GetDuck(thisVal);
        if (duck != nullptr)
        {
            const auto length = GetAnimationLength(duck->getState());
            if (length != 0)
            {
                duck->frame = static_cast<uint16_t>(value % length);
                duck->invalidate();
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScDuck::animation_get(JSContext* ctx, JSValue thisVal)
    {
        auto duck = GetDuck(thisVal);
        if (duck == nullptr)
        {
            return JSFromStdString(ctx, "");
        }
        return JSFromStdString(ctx, kDuckAnimationNames[EnumValue(duck->getState())]);
    }

    JSValue ScDuck::animation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(name, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto newState = GetDuckStateFromName(name);
        if (newState == std::nullopt)
        {
            JS_ThrowPlainError(ctx, "Invalid animation for this duck (%s)", name.data());
            return JS_EXCEPTION;
        }

        auto duck = GetDuck(thisVal);
        if (duck != nullptr)
        {
            duck->setState(*newState);
            duck->frame = 0;
            duck->invalidate();
        }
        return JS_UNDEFINED;
    }

} // namespace OpenRCT2::Scripting

#endif
