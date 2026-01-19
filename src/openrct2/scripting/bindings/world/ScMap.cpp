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
    #include "../../../entity/Fountain.h"
    #include "../../../entity/Guest.h"
    #include "../../../entity/Litter.h"
    #include "../../../entity/MoneyEffect.h"
    #include "../../../entity/Particle.h"
    #include "../../../entity/Staff.h"
    #include "../../../ride/Ride.h"
    #include "../../../ride/RideManager.hpp"
    #include "../../../ride/TrainManager.h"
    #include "../../Duktape.hpp"
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
    ScMap::ScMap(duk_context* ctx)
        : _context(ctx)
    {
    }

    DukValue ScMap::size_get() const
    {
        return ToDuk(_context, getGameState().mapSize);
    }

    int32_t ScMap::numRides_get() const
    {
        auto& gameState = getGameState();
        return static_cast<int32_t>(RideManager(gameState).size());
    }

    int32_t ScMap::numEntities_get() const
    {
        return kMaxEntities;
    }

    std::vector<std::shared_ptr<ScRide>> ScMap::rides_get() const
    {
        std::vector<std::shared_ptr<ScRide>> result;

        auto& gameState = getGameState();
        auto rideManager = RideManager(gameState);
        for (const auto& ride : rideManager)
        {
            result.push_back(std::make_shared<ScRide>(ride.id));
        }
        return result;
    }

    std::shared_ptr<ScRide> ScMap::getRide(int32_t id) const
    {
        auto& gameState = getGameState();
        auto rideManager = RideManager(gameState);
        auto ride = rideManager[RideId::FromUnderlying(id)];
        if (ride != nullptr)
        {
            return std::make_shared<ScRide>(ride->id);
        }
        return {};
    }

    std::shared_ptr<ScTile> ScMap::getTile(int32_t x, int32_t y) const
    {
        auto coords = TileCoordsXY(x, y).ToCoordsXY();
        return std::make_shared<ScTile>(coords);
    }

    DukValue ScMap::getEntity(int32_t id) const
    {
        if (id >= 0 && id < kMaxEntities)
        {
            auto spriteId = EntityId::FromUnderlying(id);
            auto sprite = getGameState().entities.GetEntity(spriteId);
            if (sprite != nullptr && sprite->Type != EntityType::null)
            {
                return GetEntityAsDukValue(sprite);
            }
        }
        duk_push_null(_context);
        return DukValue::take_from_stack(_context);
    }

    std::vector<DukValue> ScMap::getAllEntities(const std::string& type) const
    {
        std::vector<DukValue> result;
        if (type == "balloon")
        {
            for (auto sprite : EntityList<Balloon>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScBalloon>(sprite->Id)));
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

                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScVehicle>(carId)));

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
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScLitter>(sprite->Id)));
            }
        }
        else if (type == "money_effect")
        {
            for (auto sprite : EntityList<MoneyEffect>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScMoneyEffect>(sprite->Id)));
            }
        }
        else if (type == "duck")
        {
            for (auto sprite : EntityList<Duck>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(sprite->Id)));
            }
        }
        else if (type == "peep")
        {
            for (auto sprite : EntityList<Guest>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScGuest>(sprite->Id)));
            }
            for (auto sprite : EntityList<Staff>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->Id)));
            }
        }
        else if (type == "guest")
        {
            for (auto sprite : EntityList<Guest>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScGuest>(sprite->Id)));
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
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScHandyman>(sprite->Id)));
                            break;
                        case StaffType::mechanic:
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScMechanic>(sprite->Id)));
                            break;
                        case StaffType::security:
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScSecurity>(sprite->Id)));
                            break;
                        default:
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->Id)));
                            break;
                    }
                }
                else
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->Id)));
                }
            }
        }
        else if (type == "crashed_vehicle_particle")
        {
            for (auto sprite : EntityList<VehicleCrashParticle>())
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScCrashedVehicleParticle>(sprite->Id)));
            }
        }
        else
        {
            duk_error(_context, DUK_ERR_ERROR, "Invalid entity type.");
        }

        return result;
    }

    std::vector<DukValue> ScMap::getAllEntitiesOnTile(const std::string& type, const DukValue& tilePos) const
    {
        // Get the tile position
        const auto pos = FromDuk<CoordsXY>(tilePos);

        // Declare a vector that will hold the result to return
        std::vector<DukValue> result;

        // Use EntityTileList to iterate all entities of the given type on the tile, and push them to result
        if (type == "balloon")
        {
            for (auto sprite : EntityTileList<Balloon>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScBalloon>(sprite->Id)));
            }
        }
        else if (type == "car")
        {
            for (auto sprite : EntityTileList<Vehicle>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScVehicle>(sprite->Id)));
            }
        }
        else if (type == "litter")
        {
            for (auto sprite : EntityTileList<Litter>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScLitter>(sprite->Id)));
            }
        }
        else if (type == "duck")
        {
            for (auto sprite : EntityTileList<Duck>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(sprite->Id)));
            }
        }
        else if (type == "guest")
        {
            for (auto sprite : EntityTileList<Guest>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScGuest>(sprite->Id)));
            }
        }
        else if (type == "money_effect")
        {
            for (auto sprite : EntityTileList<MoneyEffect>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScMoneyEffect>(sprite->Id)));
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
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScHandyman>(sprite->Id)));
                            break;
                        case StaffType::mechanic:
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScMechanic>(sprite->Id)));
                            break;
                        case StaffType::security:
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScSecurity>(sprite->Id)));
                            break;
                        default:
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->Id)));
                            break;
                    }
                }
                else
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->Id)));
                }
            }
        }
        else if (type == "crashed_vehicle_particle")
        {
            for (auto sprite : EntityTileList<VehicleCrashParticle>(pos))
            {
                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScCrashedVehicleParticle>(sprite->Id)));
            }
        }
        else
        {
            // If the given type isn't valid, throw an error
            duk_error(_context, DUK_ERR_ERROR, "Invalid entity type: %s", type.c_str());
        }

        return result;
    }

    template<typename TEntityType, typename TScriptType>
    DukValue createEntityType(duk_context* ctx, const DukValue& initializer)
    {
        TEntityType* entity = getGameState().entities.CreateEntity<TEntityType>();
        if (entity == nullptr)
        {
            // Probably no more space for entities for this specified entity type.
            return ToDuk(ctx, undefined);
        }

        auto entityPos = CoordsXYZ{ AsOrDefault(initializer["x"], 0), AsOrDefault(initializer["y"], 0),
                                    AsOrDefault(initializer["z"], 0) };
        entity->MoveTo(entityPos);

        return GetObjectAsDukValue(ctx, std::make_shared<TScriptType>(entity->Id));
    }

    DukValue ScMap::createEntity(const std::string& type, const DukValue& initializer)
    {
        DukValue res;
        if (type == "car")
        {
            Vehicle* entity = getGameState().entities.CreateEntity<Vehicle>();
            if (entity == nullptr)
            {
                // Probably no more space for entities for this specified entity type.
                res = ToDuk(_context, undefined);
            }
            else
            {
                auto entityPos = CoordsXYZ{ AsOrDefault(initializer["x"], 0), AsOrDefault(initializer["y"], 0),
                                            AsOrDefault(initializer["z"], 0) };
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

                res = GetObjectAsDukValue(_context, std::make_shared<ScVehicle>(entity->Id));
            }
        }
        else if (type == "staff")
        {
            res = createEntityType<Staff, ScStaff>(_context, initializer);
        }
        else if (type == "guest")
        {
            res = createEntityType<Guest, ScGuest>(_context, initializer);
        }
        else if (type == "steam_particle")
        {
            res = createEntityType<SteamParticle, ScEntity>(_context, initializer);
        }
        else if (type == "money_effect")
        {
            res = createEntityType<MoneyEffect, ScMoneyEffect>(_context, initializer);
        }
        else if (type == "crashed_vehicle_particle")
        {
            res = createEntityType<VehicleCrashParticle, ScCrashedVehicleParticle>(_context, initializer);
        }
        else if (type == "explosion_cloud")
        {
            res = createEntityType<ExplosionCloud, ScEntity>(_context, initializer);
        }
        else if (type == "crash_splash")
        {
            res = createEntityType<CrashSplashParticle, ScEntity>(_context, initializer);
        }
        else if (type == "explosion_flare")
        {
            res = createEntityType<ExplosionFlare, ScEntity>(_context, initializer);
        }
        else if (type == "balloon")
        {
            res = createEntityType<Balloon, ScBalloon>(_context, initializer);
        }
        else if (type == "duck")
        {
            res = createEntityType<Duck, ScEntity>(_context, initializer);
        }
        else if (type == "jumping_fountain")
        {
            res = createEntityType<JumpingFountain, ScEntity>(_context, initializer);
        }
        else if (type == "litter")
        {
            res = createEntityType<Litter, ScLitter>(_context, initializer);
        }
        else
        {
            duk_error(_context, DUK_ERR_ERROR, "Invalid entity type.");
        }

        return res;
    }

    DukValue ScMap::getTrackIterator(const DukValue& dukPosition, int32_t elementIndex) const
    {
        auto position = FromDuk<CoordsXY>(dukPosition);
        auto trackIterator = ScTrackIterator::FromElement(position, elementIndex);
        if (trackIterator == nullptr)
            return ToDuk(_context, undefined);

        return GetObjectAsDukValue(_context, trackIterator);
    }

    void ScMap::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScMap::size_get, nullptr, "size");
        dukglue_register_property(ctx, &ScMap::numRides_get, nullptr, "numRides");
        dukglue_register_property(ctx, &ScMap::numEntities_get, nullptr, "numEntities");
        dukglue_register_property(ctx, &ScMap::rides_get, nullptr, "rides");
        dukglue_register_method(ctx, &ScMap::getRide, "getRide");
        dukglue_register_method(ctx, &ScMap::getTile, "getTile");
        dukglue_register_method(ctx, &ScMap::getEntity, "getEntity");
        dukglue_register_method(ctx, &ScMap::getAllEntities, "getAllEntities");
        dukglue_register_method(ctx, &ScMap::getAllEntitiesOnTile, "getAllEntitiesOnTile");
        dukglue_register_method(ctx, &ScMap::createEntity, "createEntity");
        dukglue_register_method(ctx, &ScMap::getTrackIterator, "getTrackIterator");
    }

    DukValue ScMap::GetEntityAsDukValue(const EntityBase* sprite) const
    {
        auto spriteId = sprite->Id;
        switch (sprite->Type)
        {
            case EntityType::vehicle:
                return GetObjectAsDukValue(_context, std::make_shared<ScVehicle>(spriteId));
            case EntityType::staff:
            {
                auto staff = getGameState().entities.GetEntity<Staff>(spriteId);
                if (staff != nullptr)
                {
                    switch (staff->AssignedStaffType)
                    {
                        case StaffType::handyman:
                            return GetObjectAsDukValue(_context, std::make_shared<ScHandyman>(spriteId));
                        case StaffType::mechanic:
                            return GetObjectAsDukValue(_context, std::make_shared<ScMechanic>(spriteId));
                        case StaffType::security:
                            return GetObjectAsDukValue(_context, std::make_shared<ScSecurity>(spriteId));
                        default:
                            return GetObjectAsDukValue(_context, std::make_shared<ScStaff>(spriteId));
                    }
                }
                else
                {
                    return GetObjectAsDukValue(_context, std::make_shared<ScStaff>(spriteId));
                }
            }
            case EntityType::guest:
                return GetObjectAsDukValue(_context, std::make_shared<ScGuest>(spriteId));
            case EntityType::litter:
                return GetObjectAsDukValue(_context, std::make_shared<ScLitter>(spriteId));
            case EntityType::balloon:
                return GetObjectAsDukValue(_context, std::make_shared<ScBalloon>(spriteId));
            case EntityType::moneyEffect:
                return GetObjectAsDukValue(_context, std::make_shared<ScMoneyEffect>(spriteId));
            case EntityType::crashedVehicleParticle:
                return GetObjectAsDukValue(_context, std::make_shared<ScCrashedVehicleParticle>(spriteId));
            default:
                return GetObjectAsDukValue(_context, std::make_shared<ScEntity>(spriteId));
        }
    }

} // namespace OpenRCT2::Scripting

#endif
