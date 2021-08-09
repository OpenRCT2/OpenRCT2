/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../common.h"
#    include "../../../ride/Ride.h"
#    include "../../../ride/TrainManager.h"
#    include "../../../world/Balloon.h"
#    include "../../../world/Duck.h"
#    include "../../../world/EntityList.h"
#    include "../../../world/Fountain.h"
#    include "../../../world/Litter.h"
#    include "../../../world/Map.h"
#    include "../../../world/MoneyEffect.h"
#    include "../../../world/Particle.h"
#    include "../../Duktape.hpp"
#    include "../entity/ScEntity.hpp"
#    include "../entity/ScVehicle.hpp"
#    include "../ride/ScRide.hpp"
#    include "../world/ScTile.hpp"

namespace OpenRCT2::Scripting
{
    class ScMap
    {
    private:
        duk_context* _context;

    public:
        ScMap(duk_context* ctx)
            : _context(ctx)
        {
        }

        DukValue size_get() const
        {
            return ToDuk(_context, CoordsXY{ gMapSize, gMapSize });
        }

        int32_t numRides_get() const
        {
            return static_cast<int32_t>(GetRideManager().size());
        }

        int32_t numEntities_get() const
        {
            return MAX_ENTITIES;
        }

        std::vector<std::shared_ptr<ScRide>> rides_get() const
        {
            std::vector<std::shared_ptr<ScRide>> result;
            auto rideManager = GetRideManager();
            for (const auto& ride : rideManager)
            {
                result.push_back(std::make_shared<ScRide>(ride.id));
            }
            return result;
        }

        std::shared_ptr<ScRide> getRide(int32_t id) const
        {
            auto rideManager = GetRideManager();
            auto ride = rideManager[static_cast<ride_id_t>(id)];
            if (ride != nullptr)
            {
                return std::make_shared<ScRide>(ride->id);
            }
            return {};
        }

        std::shared_ptr<ScTile> getTile(int32_t x, int32_t y) const
        {
            auto coords = TileCoordsXY(x, y).ToCoordsXY();
            return std::make_shared<ScTile>(coords);
        }

        DukValue getEntity(int32_t id) const
        {
            if (id >= 0 && id < MAX_ENTITIES)
            {
                auto spriteId = static_cast<uint16_t>(id);
                auto sprite = GetEntity(spriteId);
                if (sprite != nullptr && sprite->Type != EntityType::Null)
                {
                    return GetEntityAsDukValue(sprite);
                }
            }
            duk_push_null(_context);
            return DukValue::take_from_stack(_context);
        }

        std::vector<DukValue> getAllEntities(const std::string& type) const
        {
            std::vector<DukValue> result;
            if (type == "balloon")
            {
                for (auto sprite : EntityList<Balloon>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(sprite->sprite_index)));
                }
            }
            else if (type == "car")
            {
                for (auto trainHead : TrainManager::View())
                {
                    for (auto carId = trainHead->sprite_index; carId != SPRITE_INDEX_NULL;)
                    {
                        auto car = GetEntity<Vehicle>(carId);
                        result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScVehicle>(carId)));
                        carId = car->next_vehicle_on_train;
                    }
                }
            }
            else if (type == "litter")
            {
                for (auto sprite : EntityList<Litter>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(sprite->sprite_index)));
                }
            }
            else if (type == "duck")
            {
                for (auto sprite : EntityList<Duck>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(sprite->sprite_index)));
                }
            }
            else if (type == "peep")
            {
                for (auto sprite : EntityList<Guest>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScGuest>(sprite->sprite_index)));
                }
                for (auto sprite : EntityList<Staff>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->sprite_index)));
                }
            }
            else if (type == "guest")
            {
                for (auto sprite : EntityList<Guest>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScGuest>(sprite->sprite_index)));
                }
            }
            else if (type == "staff")
            {
                for (auto sprite : EntityList<Staff>())
                {
                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(sprite->sprite_index)));
                }
            }
            else
            {
                duk_error(_context, DUK_ERR_ERROR, "Invalid entity type.");
            }

            return result;
        }

        template<typename TEntityType, typename TScriptType> DukValue createEntityType(const DukValue& initializer)
        {
            TEntityType* entity = CreateEntity<TEntityType>();

            auto entityPos = CoordsXYZ{ AsOrDefault(initializer["x"], 0), AsOrDefault(initializer["y"], 0),
                                        AsOrDefault(initializer["z"], 0) };
            entity->MoveTo(entityPos);

            return GetObjectAsDukValue(_context, std::make_shared<TScriptType>(entity->sprite_index));
        }

        DukValue createEntity(const std::string& type, const DukValue& initializer)
        {
            if (type == "car")
            {
                return createEntityType<Vehicle, ScVehicle>(initializer);
            }
            else if (type == "staff")
            {
                return createEntityType<Staff, ScStaff>(initializer);
            }
            else if (type == "guest")
            {
                return createEntityType<Guest, ScGuest>(initializer);
            }
            else if (type == "steam_particle")
            {
                return createEntityType<SteamParticle, ScEntity>(initializer);
            }
            else if (type == "money_effect")
            {
                return createEntityType<MoneyEffect, ScEntity>(initializer);
            }
            else if (type == "crashed_vehicle_particle")
            {
                return createEntityType<VehicleCrashParticle, ScEntity>(initializer);
            }
            else if (type == "explosion_cloud")
            {
                return createEntityType<ExplosionCloud, ScEntity>(initializer);
            }
            else if (type == "crash_splash")
            {
                return createEntityType<CrashSplashParticle, ScEntity>(initializer);
            }
            else if (type == "explosion_flare")
            {
                return createEntityType<ExplosionFlare, ScEntity>(initializer);
            }
            else if (type == "balloon")
            {
                return createEntityType<Balloon, ScEntity>(initializer);
            }
            else if (type == "duck")
            {
                return createEntityType<Duck, ScEntity>(initializer);
            }
            else if (type == "jumping_fountain")
            {
                return createEntityType<JumpingFountain, ScEntity>(initializer);
            }
            else if (type == "litter")
            {
                return createEntityType<Litter, ScLitter>(initializer);
            }
            else
            {
                duk_error(_context, DUK_ERR_ERROR, "Invalid entity type.");
            }

            return DukValue();
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScMap::size_get, nullptr, "size");
            dukglue_register_property(ctx, &ScMap::numRides_get, nullptr, "numRides");
            dukglue_register_property(ctx, &ScMap::numEntities_get, nullptr, "numEntities");
            dukglue_register_property(ctx, &ScMap::rides_get, nullptr, "rides");
            dukglue_register_method(ctx, &ScMap::getRide, "getRide");
            dukglue_register_method(ctx, &ScMap::getTile, "getTile");
            dukglue_register_method(ctx, &ScMap::getEntity, "getEntity");
            dukglue_register_method(ctx, &ScMap::getAllEntities, "getAllEntities");
            dukglue_register_method(ctx, &ScMap::createEntity, "createEntity");
        }

    private:
        DukValue GetEntityAsDukValue(const SpriteBase* sprite) const
        {
            auto spriteId = sprite->sprite_index;
            switch (sprite->Type)
            {
                case EntityType::Vehicle:
                    return GetObjectAsDukValue(_context, std::make_shared<ScVehicle>(spriteId));
                case EntityType::Staff:
                    return GetObjectAsDukValue(_context, std::make_shared<ScStaff>(spriteId));
                case EntityType::Guest:
                    return GetObjectAsDukValue(_context, std::make_shared<ScGuest>(spriteId));
                case EntityType::Litter:
                    return GetObjectAsDukValue(_context, std::make_shared<ScLitter>(spriteId));
                default:
                    return GetObjectAsDukValue(_context, std::make_shared<ScEntity>(spriteId));
            }
        }
    };
} // namespace OpenRCT2::Scripting

#endif
