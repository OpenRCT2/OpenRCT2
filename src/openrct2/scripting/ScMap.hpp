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

#    include "../common.h"
#    include "../ride/Ride.h"
#    include "../world/Map.h"
#    include "Duktape.hpp"
#    include "ScEntity.hpp"
#    include "ScRide.hpp"
#    include "ScTile.hpp"

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
            auto ctx = _context;
            auto objIdx = duk_push_object(ctx);
            duk_push_number(ctx, gMapSize);
            duk_put_prop_string(ctx, objIdx, "x");
            duk_push_number(ctx, gMapSize);
            duk_put_prop_string(ctx, objIdx, "y");
            return DukValue::take_from_stack(ctx);
        }

        int32_t numRides_get() const
        {
            return static_cast<int32_t>(GetRideManager().size());
        }

        int32_t numEntities_get() const
        {
            return MAX_SPRITES;
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
            if (id >= 0 && id < static_cast<int32_t>(rideManager.size()))
            {
                auto ride = rideManager[static_cast<ride_id_t>(id)];
                if (ride != nullptr)
                {
                    return std::make_shared<ScRide>(ride->id);
                }
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
            if (id >= 0 && id < MAX_SPRITES)
            {
                auto spriteId = static_cast<uint16_t>(id);
                auto sprite = get_sprite(spriteId);
                if (sprite != nullptr && sprite->generic.sprite_identifier != SPRITE_IDENTIFIER_NULL)
                {
                    return GetEntityAsDukValue(sprite);
                }
            }
            duk_push_null(_context);
            return DukValue::take_from_stack(_context);
        }

        std::vector<DukValue> getAllEntities(const std::string& type) const
        {
            SPRITE_LIST targetList{};
            uint8_t targetType{};
            if (type == "balloon")
            {
                targetList = SPRITE_LIST_MISC;
                targetType = SPRITE_MISC_BALLOON;
            }
            if (type == "car")
            {
                targetList = SPRITE_LIST_TRAIN_HEAD;
            }
            else if (type == "litter")
            {
                targetList = SPRITE_LIST_LITTER;
            }
            else if (type == "duck")
            {
                targetList = SPRITE_LIST_MISC;
                targetType = SPRITE_MISC_DUCK;
            }
            else if (type == "peep")
            {
                targetList = SPRITE_LIST_PEEP;
            }
            else
            {
                duk_error(_context, DUK_ERR_ERROR, "Invalid entity type.");
            }

            std::vector<DukValue> result;
            auto spriteId = gSpriteListHead[targetList];
            while (spriteId != SPRITE_INDEX_NULL)
            {
                auto sprite = get_sprite(spriteId);
                if (sprite == nullptr)
                {
                    break;
                }
                else
                {
                    // Only the misc list checks the type property
                    if (targetList != SPRITE_LIST_MISC || sprite->generic.type == targetType)
                    {
                        if (targetList == SPRITE_LIST_PEEP)
                        {
                            if (sprite->peep.Type == PEEP_TYPE_STAFF)
                                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScStaff>(spriteId)));
                            else
                                result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScGuest>(spriteId)));
                        }
                        else if (targetList == SPRITE_LIST_TRAIN_HEAD)
                        {
                            auto carId = spriteId;
                            while (carId != SPRITE_INDEX_NULL)
                            {
                                auto car = get_sprite(carId);
                                if (car == nullptr)
                                {
                                    break;
                                }
                                else
                                {
                                    result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(carId)));
                                    carId = car->vehicle.next_vehicle_on_train;
                                }
                            }
                        }
                        else
                        {
                            result.push_back(GetObjectAsDukValue(_context, std::make_shared<ScEntity>(spriteId)));
                        }
                    }
                    spriteId = sprite->generic.next;
                }
            }
            return result;
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
        }

    private:
        DukValue GetEntityAsDukValue(const rct_sprite* sprite) const
        {
            auto spriteId = sprite->generic.sprite_index;
            switch (sprite->generic.sprite_identifier)
            {
                case SPRITE_IDENTIFIER_PEEP:
                    if (sprite->peep.Type == PEEP_TYPE_STAFF)
                        return GetObjectAsDukValue(_context, std::make_shared<ScStaff>(spriteId));
                    else
                        return GetObjectAsDukValue(_context, std::make_shared<ScGuest>(spriteId));
                default:
                    return GetObjectAsDukValue(_context, std::make_shared<ScEntity>(spriteId));
            }
        }
    };
} // namespace OpenRCT2::Scripting

#endif
