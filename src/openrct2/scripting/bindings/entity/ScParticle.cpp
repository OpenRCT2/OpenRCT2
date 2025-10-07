/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ScParticle.hpp"

#include "../../../core/EnumMap.hpp"
#include "../ride/ScRide.hpp"

#ifdef ENABLE_SCRIPTING_REFACTOR

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

    JSValue ScCrashedVehicleParticle::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = gScEntity.New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScCrashedVehicleParticle::AddFuncs(JSContext* ctx, JSValue obj)
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
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    VehicleCrashParticle* ScCrashedVehicleParticle::GetCrashedVehicleParticle(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<VehicleCrashParticle>(id);
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
            entity->Invalidate();
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
            entity->crashed_sprite_base = CrashParticleTypeMap[value];
            entity->Invalidate();
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::crashedSpriteBase_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            return JSFromStdString(ctx, CrashParticleTypeMap[entity->crashed_sprite_base]);
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
            entity->time_to_live = value;
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::timeToLive_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        return JS_NewUint32(ctx, entity == nullptr ? 0 : entity->time_to_live);
    }

    JSValue ScCrashedVehicleParticle::velocity_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_OBJECT(obj, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            auto velocity = JSToCoordsXYZ(ctx, obj);
            entity->velocity_x = velocity.x;
            entity->velocity_y = velocity.y;
            entity->velocity_z = velocity.z;
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::velocity_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            return ToJSValue(ctx, CoordsXYZ(entity->velocity_x, entity->velocity_y, entity->velocity_z));
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
            entity->acceleration_x = acceleration.x;
            entity->acceleration_y = acceleration.y;
            entity->acceleration_z = acceleration.z;
        }
        return JS_UNDEFINED;
    }
    JSValue ScCrashedVehicleParticle::acceleration_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetCrashedVehicleParticle(thisVal);
        if (entity != nullptr)
        {
            return ToJSValue(ctx, CoordsXYZ(entity->acceleration_x, entity->acceleration_y, entity->acceleration_z));
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
            entity->SetSpriteData();
            entity->Launch();

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
                entity->colour[0] = JSToUint(ctx, colours, "body");
                entity->colour[1] = JSToUint(ctx, colours, "trim");
            }
            if (JS_IsObject(acceleration))
            {
                auto accelerationXYZ = JSToCoordsXYZ(ctx, acceleration);
                entity->acceleration_x = accelerationXYZ.x;
                entity->acceleration_y = accelerationXYZ.y;
                entity->acceleration_z = accelerationXYZ.z;
            }
            if (JS_IsObject(velocity))
            {
                auto velocityXYZ = JSToCoordsXYZ(ctx, velocity);
                entity->velocity_x = velocityXYZ.x;
                entity->velocity_y = velocityXYZ.y;
                entity->velocity_z = velocityXYZ.z;
            }
            if (JS_IsNumber(timeToLive))
            {
                entity->time_to_live = JSToUint(ctx, timeToLive);
            }
            if (JS_IsNumber(frame))
            {
                entity->frame = std::clamp<uint16_t>(JSToUint(ctx, frame), 0, kCrashedVehicleParticleNumberSprites - 1)
                    * kCrashedVehicleParticleFrameToSprite;
            }
            if (JS_IsString(crashParticleType))
            {
                auto key = JSToStdString(ctx, crashParticleType);
                entity->crashed_sprite_base = CrashParticleTypeMap[key];
            }
            entity->Invalidate();

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
            JS_SetPropertyStr(ctx, obj, "body", JS_NewInt32(ctx, entity->colour[0]));
            JS_SetPropertyStr(ctx, obj, "trim", JS_NewInt32(ctx, entity->colour[1]));
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
            entity->colour[0] = JSToUint(ctx, obj, "body");
            entity->colour[1] = JSToUint(ctx, obj, "trim");
            entity->Invalidate();
        }
        return JS_UNDEFINED;
    }
} // namespace OpenRCT2::Scripting

#endif
