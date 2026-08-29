/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScParticle.hpp"

    #include "../../../GameState.h"
    #include "../../../core/EnumMap.hpp"
    #include "../../../entity/Particle.h"
    #include "../../../world/Location.hpp"

namespace OpenRCT2::Scripting
{
    static const EnumMap<uint8_t> CrashParticleTypeMap(
        {
            { "corner", 0 },
            { "rod", 1 },
            { "wheel", 2 },
            { "panel", 3 },
            { "seat", 4 },
        });

    ScCrashedVehicleParticle gScCrashedVehicleParticle;

    JSValue ScCrashedVehicleParticle::New(JSContext* ctx, EntityId entityId)
    {
        return gScEntity.NewDerivedInstance(ctx, entityId, gScCrashedVehicleParticle.GetProto());
    }

    void ScCrashedVehicleParticle::Register(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF(
                "acceleration", &ScCrashedVehicleParticle::acceleration_get, &ScCrashedVehicleParticle::acceleration_set),
            JS_CGETSET_DEF("velocity", &ScCrashedVehicleParticle::velocity_get, &ScCrashedVehicleParticle::velocity_set),
            JS_CGETSET_DEF("colours", &ScCrashedVehicleParticle::colours_get, &ScCrashedVehicleParticle::colours_set),
            JS_CGETSET_DEF("timeToLive", &ScCrashedVehicleParticle::timeToLive_get, &ScCrashedVehicleParticle::timeToLive_set),
            JS_CGETSET_DEF(
                "crashParticleType", &ScCrashedVehicleParticle::crashedSpriteBase_get,
                &ScCrashedVehicleParticle::crashedSpriteBase_set),
            JS_CGETSET_DEF("frame", &ScCrashedVehicleParticle::frame_get, &ScCrashedVehicleParticle::frame_set),
            JS_CFUNC_DEF("launch", 1, &ScCrashedVehicleParticle::Launch),
        };
        gScCrashedVehicleParticle.RegisterDerived(ctx, gScEntity, funcs);
    }

    VehicleCrashParticle* ScCrashedVehicleParticle::GetCrashedVehicleParticle(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.getEntity<VehicleCrashParticle>(id);
    }

    JSValue ScCrashedVehicleParticle::frame_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            entity->frame = std::clamp<uint16_t>(value, 0, kCrashedVehicleParticleNumberSprites - 1)
                * kCrashedVehicleParticleFrameToSprite;
            entity->invalidate();
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::frame_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        auto frame = (entity != nullptr) ? entity->frame / kCrashedVehicleParticleFrameToSprite : 0;
        return JS_NewUint32(ctx, frame);
    }

    JSValue ScCrashedVehicleParticle::crashedSpriteBase_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            entity->crashedSpriteBase = CrashParticleTypeMap[value];
            entity->invalidate();
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::crashedSpriteBase_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            return JSFromStdString(ctx, CrashParticleTypeMap[entity->crashedSpriteBase]);
        }
        return JS_UNDEFINED;
    }

    JSValue ScCrashedVehicleParticle::timeToLive_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            entity->timeToLive = value;
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::timeToLive_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        return JS_NewUint32(ctx, entity == nullptr ? 0 : entity->timeToLive);
    }

    JSValue ScCrashedVehicleParticle::velocity_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_OBJECT(obj, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            auto velocity = JSToCoordsXYZ(ctx, obj);
            entity->velocityX = velocity.x;
            entity->velocityY = velocity.y;
            entity->velocityZ = velocity.z;
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::velocity_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            return ToJSValue(ctx, CoordsXYZ(entity->velocityX, entity->velocityY, entity->velocityZ));
        }
        return JS_UNDEFINED;
    }

    JSValue ScCrashedVehicleParticle::acceleration_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_OBJECT(obj, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            auto acceleration = JSToCoordsXYZ(ctx, obj);
            entity->accelerationX = acceleration.x;
            entity->accelerationY = acceleration.y;
            entity->accelerationZ = acceleration.z;
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::acceleration_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            return ToJSValue(ctx, CoordsXYZ(entity->accelerationX, entity->accelerationY, entity->accelerationZ));
        }
        return JS_UNDEFINED;
    }

    JSValue ScCrashedVehicleParticle::Launch(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(obj, ctx, argv[0]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            entity->setSpriteData();
            entity->launch();

            if (JS_IsUndefined(obj))
                return JS_UNDEFINED;

            auto colours = JS_GetPropertyStr(ctx, obj, "colours");
            auto acceleration = JS_GetPropertyStr(ctx, obj, "acceleration");
            auto velocity = JS_GetPropertyStr(ctx, obj, "velocity");
            auto timeToLive = JS_GetPropertyStr(ctx, obj, "timeToLive");
            auto frame = JS_GetPropertyStr(ctx, obj, "frame");
            auto crashParticleType = JS_GetPropertyStr(ctx, obj, "crashParticleType");

            if (JS_IsObject(colours))
            {
                entity->colour[0] = static_cast<Drawing::Colour>(JSToUint(ctx, colours, "body"));
                entity->colour[1] = static_cast<Drawing::Colour>(JSToUint(ctx, colours, "trim"));
            }
            if (JS_IsObject(acceleration))
            {
                auto accelerationXYZ = JSToCoordsXYZ(ctx, acceleration);
                entity->accelerationX = accelerationXYZ.x;
                entity->accelerationY = accelerationXYZ.y;
                entity->accelerationZ = accelerationXYZ.z;
            }
            if (JS_IsObject(velocity))
            {
                auto velocityXYZ = JSToCoordsXYZ(ctx, velocity);
                entity->velocityX = velocityXYZ.x;
                entity->velocityY = velocityXYZ.y;
                entity->velocityZ = velocityXYZ.z;
            }
            if (JS_IsNumber(timeToLive))
            {
                entity->timeToLive = JSToUint(ctx, timeToLive);
            }
            if (JS_IsNumber(frame))
            {
                entity->frame = std::clamp<uint16_t>(JSToUint(ctx, frame), 0, kCrashedVehicleParticleNumberSprites - 1)
                    * kCrashedVehicleParticleFrameToSprite;
            }
            if (JS_IsString(crashParticleType))
            {
                auto key = JSToStdString(ctx, crashParticleType);
                entity->crashedSpriteBase = CrashParticleTypeMap[key];
            }
            entity->invalidate();

            JS_FreeValue(ctx, colours);
            JS_FreeValue(ctx, acceleration);
            JS_FreeValue(ctx, velocity);
            JS_FreeValue(ctx, timeToLive);
            JS_FreeValue(ctx, frame);
            JS_FreeValue(ctx, crashParticleType);
        }
        return JS_UNDEFINED;
    }

    JSValue ScCrashedVehicleParticle::colours_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            JSValue obj = JS_NewObject(ctx);
            JS_SetPropertyStr(ctx, obj, "body", JS_NewInt32(ctx, EnumValue(entity->colour[0])));
            JS_SetPropertyStr(ctx, obj, "trim", JS_NewInt32(ctx, EnumValue(entity->colour[1])));
            return obj;
        }
        return JS_NULL;
    }
    JSValue ScCrashedVehicleParticle::colours_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_OBJECT(obj, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            entity->colour[0] = static_cast<Drawing::Colour>(JSToUint(ctx, obj, "body"));
            entity->colour[1] = static_cast<Drawing::Colour>(JSToUint(ctx, obj, "trim"));
            entity->invalidate();
        }
        return JS_UNDEFINED;
    }
} // namespace OpenRCT2::Scripting

#endif
