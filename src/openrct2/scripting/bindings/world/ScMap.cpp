/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

    #include "ScMap.hpp"

    #include "../../../GameState.h"
    #include "../../../entity/Balloon.h"
    #include "../../../entity/Duck.h"
    #include "../../../entity/EntityList.h"
    #include "../../../entity/Guest.h"
    #include "../../../entity/JumpingFountain.h"
    #include "../../../entity/Litter.h"
    #include "../../../entity/MoneyEffect.h"
    #include "../../../entity/Particle.h"
    #include "../../../entity/Staff.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideManager.hpp"
    #include "../../../ride/TrainManager.h"
    #include "../../../ride/Vehicle.h"
    #include "../../../world/Map.h"
    #include "../../../world/tile_element/EntranceElement.h"
    #include "../../../world/tile_element/PathElement.h"
    #include "../../../world/tile_element/TileElement.h"
    #include "../../../world/tile_element/TrackElement.h"
    #include "../../ScriptUtil.hpp"
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

    static std::optional<std::tuple<int32_t, int32_t, int32_t, int32_t>> ParseMapBounds(JSContext* ctx, JSValue boundsVal)
    {
        if (!JS_IsObject(boundsVal))
        {
            return std::nullopt;
        }
        auto minX = JSToOptionalInt(ctx, boundsVal, "minX");
        auto minY = JSToOptionalInt(ctx, boundsVal, "minY");
        auto maxX = JSToOptionalInt(ctx, boundsVal, "maxX");
        auto maxY = JSToOptionalInt(ctx, boundsVal, "maxY");
        if (!minX || !minY || !maxX || !maxY)
        {
            return std::nullopt;
        }
        if (*minX > *maxX || *minY > *maxY)
        {
            return std::nullopt;
        }
        return std::make_tuple(*minX, *minY, *maxX, *maxY);
    }

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
            if (sprite != nullptr && sprite->type != EntityType::null)
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
                JS_SetPropertyInt64(ctx, result, idx++, ScBalloon::New(ctx, sprite->id));
            }
        }
        else if (type == "car")
        {
            for (auto trainHead : TrainManager::View())
            {
                for (auto carId = trainHead->id; !carId.IsNull();)
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
                JS_SetPropertyInt64(ctx, result, idx++, ScLitter::New(ctx, sprite->id));
            }
        }
        else if (type == "money_effect")
        {
            for (auto sprite : EntityList<MoneyEffect>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScMoneyEffect::New(ctx, sprite->id));
            }
        }
        else if (type == "duck")
        {
            for (auto sprite : EntityList<Duck>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, gScEntity.New(ctx, sprite->id));
            }
        }
        else if (type == "peep")
        {
            for (auto sprite : EntityList<Guest>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, sprite->id));
            }
            for (auto sprite : EntityList<Staff>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->id));
            }
        }
        else if (type == "guest")
        {
            for (auto sprite : EntityList<Guest>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, sprite->id));
            }
        }
        else if (type == "staff")
        {
            for (auto sprite : EntityList<Staff>())
            {
                auto staff = getGameState().entities.GetEntity<Staff>(sprite->id);
                if (staff != nullptr)
                {
                    switch (staff->assignedStaffType)
                    {
                        case StaffType::handyman:
                            JS_SetPropertyInt64(ctx, result, idx++, ScHandyman::New(ctx, sprite->id));
                            break;
                        case StaffType::mechanic:
                            JS_SetPropertyInt64(ctx, result, idx++, ScMechanic::New(ctx, sprite->id));
                            break;
                        case StaffType::security:
                            JS_SetPropertyInt64(ctx, result, idx++, ScSecurity::New(ctx, sprite->id));
                            break;
                        default:
                            JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->id));
                            break;
                    }
                }
                else
                {
                    JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->id));
                }
            }
        }
        else if (type == "crashed_vehicle_particle")
        {
            for (auto sprite : EntityList<VehicleCrashParticle>())
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScCrashedVehicleParticle::New(ctx, sprite->id));
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
                JS_SetPropertyInt64(ctx, result, idx++, ScBalloon::New(ctx, sprite->id));
            }
        }
        else if (type == "car")
        {
            for (auto sprite : EntityTileList<Vehicle>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScVehicle::New(ctx, sprite->id));
            }
        }
        else if (type == "litter")
        {
            for (auto sprite : EntityTileList<Litter>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScLitter::New(ctx, sprite->id));
            }
        }
        else if (type == "duck")
        {
            for (auto sprite : EntityTileList<Duck>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScEntity::New(ctx, sprite->id));
            }
        }
        else if (type == "guest")
        {
            for (auto sprite : EntityTileList<Guest>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, sprite->id));
            }
        }
        else if (type == "money_effect")
        {
            for (auto sprite : EntityTileList<MoneyEffect>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScMoneyEffect::New(ctx, sprite->id));
            }
        }
        else if (type == "staff")
        {
            for (auto sprite : EntityTileList<Staff>(pos))
            {
                auto staff = getGameState().entities.GetEntity<Staff>(sprite->id);
                if (staff != nullptr)
                {
                    switch (staff->assignedStaffType)
                    {
                        case StaffType::handyman:
                            JS_SetPropertyInt64(ctx, result, idx++, ScHandyman::New(ctx, sprite->id));
                            break;
                        case StaffType::mechanic:
                            JS_SetPropertyInt64(ctx, result, idx++, ScMechanic::New(ctx, sprite->id));
                            break;
                        case StaffType::security:
                            JS_SetPropertyInt64(ctx, result, idx++, ScSecurity::New(ctx, sprite->id));
                            break;
                        default:
                            JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->id));
                            break;
                    }
                }
                else
                {
                    JS_SetPropertyInt64(ctx, result, idx++, ScStaff::New(ctx, sprite->id));
                }
            }
        }
        else if (type == "crashed_vehicle_particle")
        {
            for (auto sprite : EntityTileList<VehicleCrashParticle>(pos))
            {
                JS_SetPropertyInt64(ctx, result, idx++, ScCrashedVehicleParticle::New(ctx, sprite->id));
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
        entity->moveTo(entityPos);

        return TScriptType::New(ctx, entity->id);
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
                entity->moveTo(entityPos);

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

                res = ScVehicle::New(ctx, entity->id);
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

    JSValue ScMap::getElementsInRect(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(type, ctx, argv[0]);
        auto bounds = ParseMapBounds(ctx, argv[1]);
        if (!bounds)
        {
            JS_ThrowPlainError(ctx, "Invalid bounds.");
            return JS_EXCEPTION;
        }

        auto [minX, minY, maxX, maxY] = *bounds;
        const auto& mapSize = getGameState().mapSize;
        minX = std::clamp(minX, 0, mapSize.x - 1);
        minY = std::clamp(minY, 0, mapSize.y - 1);
        maxX = std::clamp(maxX, 0, mapSize.x - 1);
        maxY = std::clamp(maxY, 0, mapSize.y - 1);

        JSValue result = JS_NewArray(ctx);
        int64_t idx = 0;

        const bool wantFootpath = type == "footpath";
        const bool wantTrack = type == "track";
        const bool wantEntrance = type == "entrance";
        if (!wantFootpath && !wantTrack && !wantEntrance)
        {
            JS_FreeValue(ctx, result);
            JS_ThrowPlainError(ctx, "Invalid element type.");
            return JS_EXCEPTION;
        }

        for (int32_t x = minX; x <= maxX; ++x)
        {
            for (int32_t y = minY; y <= maxY; ++y)
            {
                const auto coords = TileCoordsXY(x, y).ToCoordsXY();
                for (auto* element = MapGetFirstElementAt(coords); element != nullptr; element += 1)
                {
                    const auto elementType = element->getType();
                    if (wantFootpath && elementType == TileElementType::Path)
                    {
                        auto* path = element->asPath();
                        JSValue obj = JS_NewObject(ctx);
                        JS_SetPropertyStr(ctx, obj, "tileX", JS_NewInt32(ctx, x));
                        JS_SetPropertyStr(ctx, obj, "tileY", JS_NewInt32(ctx, y));
                        JS_SetPropertyStr(ctx, obj, "baseZ", JS_NewInt32(ctx, path->getBaseZ()));
                        JS_SetPropertyStr(ctx, obj, "isQueue", JS_NewBool(ctx, path->IsQueue()));
                        if (path->HasAddition())
                        {
                            const auto status = path->GetAdditionStatus();
                            JS_SetPropertyStr(ctx, obj, "additionStatus", JS_NewUint32(ctx, status));
                            JS_SetPropertyStr(ctx, obj, "isAdditionBroken", JS_NewBool(ctx, path->IsBroken()));
                            JS_SetPropertyStr(ctx, obj, "isAdditionFull", JS_NewBool(ctx, status < 255));
                        }
                        JS_SetPropertyInt64(ctx, result, idx++, obj);
                    }
                    else if (wantTrack && elementType == TileElementType::Track)
                    {
                        auto* track = element->asTrack();
                        JSValue obj = JS_NewObject(ctx);
                        JS_SetPropertyStr(ctx, obj, "tileX", JS_NewInt32(ctx, x));
                        JS_SetPropertyStr(ctx, obj, "tileY", JS_NewInt32(ctx, y));
                        JS_SetPropertyStr(ctx, obj, "baseZ", JS_NewInt32(ctx, track->getBaseZ()));
                        JS_SetPropertyStr(ctx, obj, "trackType", JS_NewInt32(ctx, EnumValue(track->GetTrackType())));
                        JS_SetPropertyStr(ctx, obj, "ride", JS_NewInt32(ctx, track->GetRideIndex().ToUnderlying()));
                        JS_SetPropertyStr(ctx, obj, "sequenceIndex", JS_NewUint32(ctx, track->GetSequenceIndex()));
                        JS_SetPropertyInt64(ctx, result, idx++, obj);
                    }
                    else if (wantEntrance && elementType == TileElementType::Entrance)
                    {
                        auto* entrance = element->asEntrance();
                        JSValue obj = JS_NewObject(ctx);
                        JS_SetPropertyStr(ctx, obj, "tileX", JS_NewInt32(ctx, x));
                        JS_SetPropertyStr(ctx, obj, "tileY", JS_NewInt32(ctx, y));
                        JS_SetPropertyStr(ctx, obj, "baseZ", JS_NewInt32(ctx, entrance->getBaseZ()));
                        JS_SetPropertyStr(ctx, obj, "ride", JS_NewInt32(ctx, entrance->GetRideIndex().ToUnderlying()));
                        JS_SetPropertyStr(ctx, obj, "station", JS_NewInt32(ctx, entrance->GetStationIndex().ToUnderlying()));
                        JS_SetPropertyStr(
                            ctx, obj, "isExit",
                            JS_NewBool(ctx, entrance->GetEntranceType() == OpenRCT2::ENTRANCE_TYPE_RIDE_EXIT));
                        JS_SetPropertyInt64(ctx, result, idx++, obj);
                    }

                    if (element->isLastForTile())
                    {
                        break;
                    }
                }
            }
        }

        return result;
    }

    JSValue ScMap::getGuestsInRect(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto bounds = ParseMapBounds(ctx, argv[0]);
        if (!bounds)
        {
            JS_ThrowPlainError(ctx, "Invalid bounds.");
            return JS_EXCEPTION;
        }

        auto [minX, minY, maxX, maxY] = *bounds;
        const auto& mapSize = getGameState().mapSize;
        minX = std::clamp(minX, 0, mapSize.x - 1);
        minY = std::clamp(minY, 0, mapSize.y - 1);
        maxX = std::clamp(maxX, 0, mapSize.x - 1);
        maxY = std::clamp(maxY, 0, mapSize.y - 1);

        JSValue result = JS_NewArray(ctx);
        int64_t idx = 0;

        for (int32_t x = minX; x <= maxX; ++x)
        {
            for (int32_t y = minY; y <= maxY; ++y)
            {
                const auto coords = TileCoordsXY(x, y).ToCoordsXY();
                for (auto* guest : EntityTileList<Guest>(coords))
                {
                    JS_SetPropertyInt64(ctx, result, idx++, ScGuest::New(ctx, guest->id));
                }
            }
        }

        return result;
    }

    void ScMap::Register(JSContext* ctx)
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
            JS_CFUNC_DEF("getElementsInRect", 2, ScMap::getElementsInRect),
            JS_CFUNC_DEF("getGuestsInRect", 1, ScMap::getGuestsInRect),
        };
        RegisterBase(ctx, "Map", nullptr, funcs);
    }

    JSValue ScMap::New(JSContext* ctx)
    {
        return MakeWithOpaque(ctx, nullptr);
    }

    JSValue ScMap::GetEntityAsDukValue(JSContext* ctx, const EntityBase* sprite)
    {
        auto spriteId = sprite->id;
        switch (sprite->type)
        {
            case EntityType::vehicle:
                return ScVehicle::New(ctx, spriteId);
            case EntityType::staff:
            {
                auto staff = getGameState().entities.GetEntity<Staff>(spriteId);
                if (staff != nullptr)
                {
                    switch (staff->assignedStaffType)
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
