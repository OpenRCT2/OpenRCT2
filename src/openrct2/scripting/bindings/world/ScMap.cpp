/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScMap.hpp"

    #include "../../../GameState.h"
    #include "../../../entity/Balloon.h"
    #include "../../../entity/Duck.h"
    #include "../../../entity/EntityList.h"
    #include "../../../entity/Fountain.h"
    #include "../../../entity/Guest.h"
    #include "../../../entity/Litter.h"
    #include "../../../entity/MoneyEffect.h"
    #include "../../../entity/Particle.h"
    #include "../../../entity/Staff.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideManager.hpp"
    #include "../../../ride/TrainManager.h"
    #include "../../../ride/Vehicle.h"
    #include "../entity/ScBalloon.hpp"
    #include "../entity/ScEntity.hpp"
    #include "../entity/ScGuest.hpp"
    #include "../entity/ScLitter.hpp"
    #include "../entity/ScMoneyEffect.hpp"
    #include "../entity/ScParticle.hpp"
    #include "../entity/ScStaff.hpp"
    #include "../entity/ScVehicle.hpp"
    #include "../ride/ScRide.hpp"
    #include "../ride/ScTrackIterator.h"
    #include "../world/ScTile.hpp"

namespace OpenRCT2::Scripting
{

    JSValue ScMap::size_get(JSContext* ctx, JSValue thisVal)
    {
        return ToJSValue(ctx, getGameState().mapSize);
    }

    JSValue ScMap::numRides_get(JSContext* ctx, JSValue thisVal)
    {
        auto& gameState = getGameState();
        return JS_NewInt64(ctx, RideManager(gameState).size());
    }

    JSValue ScMap::numEntities_get(JSContext* ctx, JSValue thisVal)
    {
        return JS_NewInt32(ctx, kMaxEntities);
    }

    JSValue ScMap::rides_get(JSContext* ctx, JSValue thisVal)
    {
        JSValue result = JS_NewArray(ctx);

        auto& gameState = getGameState();
        auto rideManager = RideManager(gameState);
        int64_t idx = 0;
        for (const auto& ride : rideManager)
        {
            JS_SetPropertyInt64(ctx, result, idx++, gScRide.New(ctx, ride.id));
        }
        return result;
    }

    JSValue ScMap::getRide(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_INT32(id, ctx, argv[0]);
        auto& gameState = getGameState();
        auto rideManager = RideManager(gameState);
        auto ride = rideManager[RideId::FromUnderlying(id)];
        if (ride != nullptr)
        {
            return gScRide.New(ctx, ride->id);
        }
        return JS_NULL;
    }

    JSValue ScMap::getTile(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_INT32(x, ctx, argv[0]);
        JS_UNPACK_INT32(y, ctx, argv[1]);
        auto coords = TileCoordsXY(x, y).ToCoordsXY();
        return gScTile.New(ctx, coords);
    }

    JSValue ScMap::getEntity(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_INT32(id, ctx, argv[0]);

        if (id >= 0 && id < kMaxEntities)
        {
            auto spriteId = EntityId::FromUnderlying(id);
            auto sprite = getGameState().entities.GetEntity(spriteId);
            if (sprite != nullptr && sprite->Type != EntityType::null)
            {
                return GetEntityAsDukValue(ctx, sprite);
            }
        }
        return JS_NULL;
    }

    JSValue ScMap::getAllEntities(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(type, ctx, argv[0]);

        JSValue result = JS_NewArray(ctx);
        uint64_t idx = 0;
        if (type == "balloon")
        {
            for (auto sprite : EntityList<Balloon>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScBalloon::New(ctx, sprite->Id));
            }
        }
        else if (type == "car")
        {
            for (auto trainHead : TrainManager::View())
            {
                for (auto carId = trainHead->Id; !carId.IsNull();)
                {
                    auto car = getGameState().entities.GetEntity<Vehicle>(carId);

                    if (car == nullptr)
                    {
                        break;
                    }

                    JS_SetPropertyInt64(ctx, result, idx++, ScVehicle::New(ctx, carId));

                    // Prevent infinite loops: Ensure next_vehicle_on_train is valid and not self-referencing
                    auto nextCarId = car->next_vehicle_on_train;
                    if (nextCarId == carId)
                    {
                        break;
                    }

                    carId = nextCarId;
                }
            }
        }
        else if (type == "litter")
        {
            for (auto sprite : EntityList<Litter>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScLitter::New(ctx, sprite->Id));
            }
        }
        else if (type == "money_effect")
        {
            for (auto sprite : EntityList<MoneyEffect>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScMoneyEffect::New(ctx, sprite->Id));
            }
        }
        else if (type == "duck")
        {
            for (auto sprite : EntityList<Duck>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, gScEntity.New(ctx, sprite->Id));
            }
        }
        else if (type == "peep")
        {
            for (auto sprite : EntityList<Guest>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, sprite->Id));
            }
            for (auto sprite : EntityList<Staff>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->Id));
            }
        }
        else if (type == "guest")
        {
            for (auto sprite : EntityList<Guest>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, sprite->Id));
            }
        }
        else if (type == "staff")
        {
            for (auto sprite : EntityList<Staff>())
            {
                auto staff = getGameState().entities.GetEntity<Staff>(sprite->Id);
                if (staff != nullptr)
                {
                    switch (staff->AssignedStaffType)
                    {
                        case StaffType::handyman:
                            JS_SetPropertyInt64(ctx, result, idx++, ScHandyman::New(ctx, sprite->Id));
                            break;
                        case StaffType::mechanic:
                            JS_SetPropertyInt64(ctx, result, idx++, ScMechanic::New(ctx, sprite->Id));
                            break;
                        case StaffType::security:
                            JS_SetPropertyInt64(ctx, result, idx++, ScSecurity::New(ctx, sprite->Id));
                            break;
                        default:
                            JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->Id));
                            break;
                    }
                }
                else
                {
                    JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->Id));
                }
            }
        }
        else if (type == "crashed_vehicle_particle")
        {
            for (auto sprite : EntityList<VehicleCrashParticle>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScCrashedVehicleParticle::New(ctx, sprite->Id));
            }
        }
        else
        {
            JS_FreeValue(ctx, result);
            JS_ThrowPlainError(ctx, "Invalid entity type.");
            return JS_EXCEPTION;
        }

        return result;
    }

    JSValue ScMap::getAllEntitiesOnTile(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(type, ctx, argv[0]);

        // Get the tile position
        const auto pos = JSToCoordsXY(ctx, argv[1]);

        // Declare an array that will hold the result to return
        JSValue result = JS_NewArray(ctx);
        uint64_t idx = 0;

        // Use EntityTileList to iterate all entities of the given type on the tile, and push them to result
        if (type == "balloon")
        {
            for (auto sprite : EntityTileList<Balloon>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScBalloon::New(ctx, sprite->Id));
            }
        }
        else if (type == "car")
        {
            for (auto sprite : EntityTileList<Vehicle>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScVehicle::New(ctx, sprite->Id));
            }
        }
        else if (type == "litter")
        {
            for (auto sprite : EntityTileList<Litter>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScLitter::New(ctx, sprite->Id));
            }
        }
        else if (type == "duck")
        {
            for (auto sprite : EntityTileList<Duck>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScEntity::New(ctx, sprite->Id));
            }
        }
        else if (type == "guest")
        {
            for (auto sprite : EntityTileList<Guest>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, sprite->Id));
            }
        }
        else if (type == "money_effect")
        {
            for (auto sprite : EntityTileList<MoneyEffect>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScMoneyEffect::New(ctx, sprite->Id));
            }
        }
        else if (type == "staff")
        {
            for (auto sprite : EntityTileList<Staff>(pos))
            {
                auto staff = getGameState().entities.GetEntity<Staff>(sprite->Id);
                if (staff != nullptr)
                {
                    switch (staff->AssignedStaffType)
                    {
                        case StaffType::handyman:
                            JS_SetPropertyInt64(ctx, result, idx++, ScHandyman::New(ctx, sprite->Id));
                            break;
                        case StaffType::mechanic:
                            JS_SetPropertyInt64(ctx, result, idx++, ScMechanic::New(ctx, sprite->Id));
                            break;
                        case StaffType::security:
                            JS_SetPropertyInt64(ctx, result, idx++, ScSecurity::New(ctx, sprite->Id));
                            break;
                        default:
                            JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->Id));
                            break;
                    }
                }
                else
                {
                    JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->Id));
                }
            }
        }
        else if (type == "crashed_vehicle_particle")
        {
            for (auto sprite : EntityTileList<VehicleCrashParticle>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScCrashedVehicleParticle::New(ctx, sprite->Id));
            }
        }
        else
        {
            // If the given type isn't valid, throw an error
            JS_FreeValue(ctx, result);
            JS_ThrowPlainError(ctx, "Invalid entity type: %s", type.c_str());
            return JS_EXCEPTION;
        }

        return result;
    }

    template<typename TEntityType, typename TScriptType>
    JSValue createEntityType(JSContext* ctx, JSValue initializer)
    {
        TEntityType* entity = getGameState().entities.CreateEntity<TEntityType>();
        if (entity == nullptr)
        {
            // Probably no more space for entities for this specified entity type.
            return JS_UNDEFINED;
        }

        auto entityPos = CoordsXYZ{ AsOrDefault(ctx, initializer, "x", 0), AsOrDefault(ctx, initializer, "y", 0),
                                    AsOrDefault(ctx, initializer, "z", 0) };
        entity->MoveTo(entityPos);

        return TScriptType::New(ctx, entity->Id);
    }

    JSValue ScMap::createEntity(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(type, ctx, argv[0]);
        JSValue initializer = argv[1];

        JSValue res;
        if (type == "car")
        {
            Vehicle* entity = getGameState().entities.CreateEntity<Vehicle>();
            if (entity == nullptr)
            {
                // Probably no more space for entities for this specified entity type.
                res = JS_UNDEFINED;
            }
            else
            {
                auto entityPos = CoordsXYZ{ AsOrDefault(ctx, initializer, "x", 0), AsOrDefault(ctx, initializer, "y", 0),
                                            AsOrDefault(ctx, initializer, "z", 0) };
                entity->MoveTo(entityPos);

                // Reset some important vehicle vars to their null values
                entity->sound1_id = Audio::SoundId::null;
                entity->sound2_id = Audio::SoundId::null;
                entity->next_vehicle_on_train = EntityId::GetNull();
                entity->scream_sound_id = Audio::SoundId::null;
                for (size_t i = 0; i < std::size(entity->peep); i++)
                {
                    entity->peep[i] = EntityId::GetNull();
                }
                entity->BoatLocation.SetNull();

                res = ScVehicle::New(ctx, entity->Id);
            }
        }
        else if (type == "staff")
        {
            res = createEntityType<Staff, ScStaff>(ctx, initializer);
        }
        else if (type == "guest")
        {
            res = createEntityType<Guest, ScGuest>(ctx, initializer);
        }
        else if (type == "steam_particle")
        {
            res = createEntityType<SteamParticle, ScEntity>(ctx, initializer);
        }
        else if (type == "money_effect")
        {
            res = createEntityType<MoneyEffect, ScMoneyEffect>(ctx, initializer);
        }
        else if (type == "crashed_vehicle_particle")
        {
            res = createEntityType<VehicleCrashParticle, ScCrashedVehicleParticle>(ctx, initializer);
        }
        else if (type == "explosion_cloud")
        {
            res = createEntityType<ExplosionCloud, ScEntity>(ctx, initializer);
        }
        else if (type == "crash_splash")
        {
            res = createEntityType<CrashSplashParticle, ScEntity>(ctx, initializer);
        }
        else if (type == "explosion_flare")
        {
            res = createEntityType<ExplosionFlare, ScEntity>(ctx, initializer);
        }
        else if (type == "balloon")
        {
            res = createEntityType<Balloon, ScBalloon>(ctx, initializer);
        }
        else if (type == "duck")
        {
            res = createEntityType<Duck, ScEntity>(ctx, initializer);
        }
        else if (type == "jumping_fountain")
        {
            res = createEntityType<JumpingFountain, ScEntity>(ctx, initializer);
        }
        else if (type == "litter")
        {
            res = createEntityType<Litter, ScLitter>(ctx, initializer);
        }
        else
        {
            JS_ThrowPlainError(ctx, "Invalid entity type.");
            res = JS_EXCEPTION;
        }

        return res;
    }

    JSValue ScMap::getTrackIterator(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(pos, ctx, argv[0]);
        JS_UNPACK_UINT32(elementIndex, ctx, argv[1]);

        const auto position = JSToCoordsXY(ctx, pos);
        return ScTrackIterator::FromElement(ctx, position, elementIndex);
    }

    void ScMap::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Map");
    }

    JSValue ScMap::New(JSContext* ctx)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("size", ScMap::size_get, nullptr),
            JS_CGETSET_DEF("numRides", ScMap::numRides_get, nullptr),
            JS_CGETSET_DEF("numEntities", ScMap::numEntities_get, nullptr),
            JS_CGETSET_DEF("rides", ScMap::rides_get, nullptr),
            JS_CFUNC_DEF("getRide", 1, ScMap::getRide),
            JS_CFUNC_DEF("getTile", 2, ScMap::getTile),
            JS_CFUNC_DEF("getEntity", 1, ScMap::getEntity),
            JS_CFUNC_DEF("getAllEntities", 1, ScMap::getAllEntities),
            JS_CFUNC_DEF("getAllEntitiesOnTile", 2, ScMap::getAllEntitiesOnTile),
            JS_CFUNC_DEF("createEntity", 2, ScMap::createEntity),
            JS_CFUNC_DEF("getTrackIterator", 2, ScMap::getTrackIterator),
        };
        return MakeWithOpaque(ctx, funcs, nullptr);
    }

    JSValue ScMap::GetEntityAsDukValue(JSContext* ctx, const EntityBase* sprite)
    {
        auto spriteId = sprite->Id;
        switch (sprite->Type)
        {
            case EntityType::vehicle:
                return ScVehicle::New(ctx, spriteId);
            case EntityType::staff:
            {
                auto staff = getGameState().entities.GetEntity<Staff>(spriteId);
                if (staff != nullptr)
                {
                    switch (staff->AssignedStaffType)
                    {
                        case StaffType::handyman:
                            return ScHandyman::New(ctx, spriteId);
                        case StaffType::mechanic:
                            return ScMechanic::New(ctx, spriteId);
                        case StaffType::security:
                            return ScSecurity::New(ctx, spriteId);
                        default:
                            return ScStaff::New(ctx, spriteId);
                    }
                }
                else
                {
                    return ScStaff::New(ctx, spriteId);
                }
            }
            case EntityType::guest:
                return ScGuest::New(ctx, spriteId);
            case EntityType::litter:
                return ScLitter::New(ctx, spriteId);
            case EntityType::balloon:
                return ScBalloon::New(ctx, spriteId);
            case EntityType::moneyEffect:
                return ScMoneyEffect::New(ctx, spriteId);
            case EntityType::crashedVehicleParticle:
                return ScCrashedVehicleParticle::New(ctx, spriteId);
            default:
                return ScEntity::New(ctx, spriteId);
        }
    }

} // namespace OpenRCT2::Scripting

#endif
