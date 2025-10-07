/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScEntity.hpp"

    #include "../../../entity/Staff.h"
    #include "ScBalloon.hpp"
    #include "ScGuest.hpp"
    #include "ScLitter.hpp"
    #include "ScMoneyEffect.hpp"
    #include "ScParticle.hpp"
    #include "ScPeep.hpp"
    #include "ScStaff.hpp"
    #include "ScVehicle.hpp"

namespace OpenRCT2::Scripting
{
    static inline std::string EntityTypeToString(const EntityBase* entity)
    {
        const auto targetApiVersion = GetTargetAPIVersion();

        if (entity != nullptr)
        {
            switch (entity->Type)
            {
                case EntityType::vehicle:
                    return "car";
                case EntityType::guest:
                    if (targetApiVersion <= kApiVersionPeepDeprecation)
                        return "peep";
                    return "guest";
                case EntityType::staff:
                    if (targetApiVersion <= kApiVersionPeepDeprecation)
                        return "peep";
                    return "staff";
                case EntityType::steamParticle:
                    return "steam_particle";
                case EntityType::moneyEffect:
                    return "money_effect";
                case EntityType::crashedVehicleParticle:
                    return "crashed_vehicle_particle";
                case EntityType::explosionCloud:
                    return "explosion_cloud";
                case EntityType::crashSplash:
                    return "crash_splash";
                case EntityType::explosionFlare:
                    return "explosion_flare";
                case EntityType::balloon:
                    return "balloon";
                case EntityType::duck:
                    return "duck";
                case EntityType::jumpingFountain:
                    return "jumping_fountain";
                case EntityType::litter:
                    return "litter";
                case EntityType::null:
                    return "unknown";
                default:
                    break;
            }
        }
        return "unknown";
    }

    using OpaqueEntityData = struct
    {
        EntityId id;
    };

    JSValue ScEntity::id_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetEntity(thisVal);
        if (entity == nullptr)
            return JS_UNDEFINED;

        return JS_NewInt32(ctx, entity->Id.ToUnderlying());
    }

    JSValue ScEntity::type_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetEntity(thisVal);
        auto type = EntityTypeToString(entity);
        return JSFromStdString(ctx, type);
    }

    // x getter and setter
    JSValue ScEntity::x_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetEntity(thisVal);
        return JS_NewInt32(ctx, entity != nullptr ? entity->x : 0);
    }
    JSValue ScEntity::x_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetEntity(thisVal);
        if (entity != nullptr)
        {
            entity->MoveTo({ value, entity->y, entity->z });
        }
        return JS_UNDEFINED;
    }

    // y getter and setter
    JSValue ScEntity::y_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetEntity(thisVal);
        return JS_NewInt32(ctx, entity != nullptr ? entity->y : 0);
    }
    JSValue ScEntity::y_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetEntity(thisVal);
        if (entity != nullptr)
        {
            entity->MoveTo({ entity->x, value, entity->z });
        }
        return JS_UNDEFINED;
    }

    // z getter and setter
    JSValue ScEntity::z_get(JSContext* ctx, JSValue thisVal)
    {
        auto entity = GetEntity(thisVal);
        return JS_NewInt32(ctx, entity != nullptr ? entity->z : 0);
    }
    JSValue ScEntity::z_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto entity = GetEntity(thisVal);
        if (entity != nullptr)
        {
            entity->MoveTo({ entity->x, entity->y, value });
        }
        return JS_UNDEFINED;
    }

    JSValue ScEntity::remove(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto entity = GetEntity(thisVal);
        if (entity != nullptr)
        {
            entity->Invalidate();
            switch (entity->Type)
            {
                case EntityType::vehicle:
                    JS_ThrowPlainError(ctx, "Removing a vehicle is currently unsupported.");
                    return JS_EXCEPTION;
                case EntityType::guest:
                case EntityType::staff:
                {
                    auto peep = entity->As<Peep>();
                    // We can't remove a single peep from a ride at the moment as this can cause complications with the
                    // vehicle car having an unsupported peep capacity.
                    if (peep == nullptr || peep->State == PeepState::onRide || peep->State == PeepState::enteringRide)
                    {
                        JS_ThrowPlainError(ctx, "Removing a peep that is on a ride is currently unsupported.");
                        return JS_EXCEPTION;
                    }
                    peep->Remove();
                    break;
                }
                case EntityType::steamParticle:
                case EntityType::moneyEffect:
                case EntityType::crashedVehicleParticle:
                case EntityType::explosionCloud:
                case EntityType::crashSplash:
                case EntityType::explosionFlare:
                case EntityType::jumpingFountain:
                case EntityType::balloon:
                case EntityType::duck:
                case EntityType::litter:
                    getGameState().entities.EntityRemove(entity);
                    break;
                case EntityType::null:
                    break;
                default:
                    break;
            }
        }
        return JS_UNDEFINED;
    }

    EntityId ScEntity::GetEntityId(JSValue thisVal)
    {
        return gScEntity.GetOpaque<OpaqueEntityData*>(thisVal)->id;
    }

    EntityBase* ScEntity::GetEntity(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return OpenRCT2::getGameState().entities.GetEntity(id);
    }

    JSValue ScEntity::NewInstance(JSContext* ctx, EntityId entityId)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("id", &ScEntity::id_get, nullptr),        JS_CGETSET_DEF("type", &ScEntity::type_get, nullptr),
            JS_CGETSET_DEF("x", &ScEntity::x_get, &ScEntity::x_set), JS_CGETSET_DEF("y", &ScEntity::y_get, &ScEntity::y_set),
            JS_CGETSET_DEF("z", &ScEntity::z_get, &ScEntity::z_set), JS_CFUNC_DEF("remove", 0, &ScEntity::remove)
        };
        return MakeWithOpaque(ctx, funcs, new OpaqueEntityData{ entityId });
    }

    // this one exists as a hack to make a template work in ScMap
    JSValue ScEntity::New(JSContext* ctx, EntityId entityId)
    {
        return gScEntity.NewInstance(ctx, entityId);
    }

    void ScEntity::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Entity", Finalize);
    }

    void ScEntity::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueEntityData* data = gScEntity.GetOpaque<OpaqueEntityData*>(thisVal);
        if (data)
            delete data;
    }
} // namespace OpenRCT2::Scripting

#endif
