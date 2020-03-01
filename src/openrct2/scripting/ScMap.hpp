/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef __ENABLE_SCRIPTING__

#    include "../common.h"
#    include "../ride/Ride.h"
#    include "../world/Map.h"
#    include "Duktape.hpp"
#    include "ScRide.hpp"
#    include "ScThing.hpp"
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

        DukValue size_get()
        {
            auto ctx = _context;
            auto objIdx = duk_push_object(ctx);
            duk_push_number(ctx, gMapSize);
            duk_put_prop_string(ctx, objIdx, "x");
            duk_push_number(ctx, gMapSize);
            duk_put_prop_string(ctx, objIdx, "y");
            return DukValue::take_from_stack(ctx);
        }

        int32_t numRides_get()
        {
            return static_cast<int32_t>(GetRideManager().size());
        }

        int32_t numThings_get()
        {
            return MAX_SPRITES;
        }

        std::vector<std::shared_ptr<ScRide>> rides_get()
        {
            std::vector<std::shared_ptr<ScRide>> result;
            auto rideManager = GetRideManager();
            for (const auto& ride : rideManager)
            {
                result.push_back(std::make_shared<ScRide>(ride.id));
            }
            return result;
        }

        std::shared_ptr<ScRide> getRide(int32_t id)
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

        std::shared_ptr<ScTile> getTile(int32_t x, int32_t y)
        {
            auto coords = TileCoordsXY(x, y).ToCoordsXY();
            return std::make_shared<ScTile>(coords);
        }

        std::shared_ptr<ScThing> getThing(int32_t id)
        {
            if (id >= 0 && id < MAX_SPRITES)
            {
                auto spriteId = static_cast<uint16_t>(id);
                auto sprite = get_sprite(spriteId);
                if (sprite != nullptr && sprite->generic.sprite_identifier != SPRITE_IDENTIFIER_NULL)
                {
                    return std::make_shared<ScThing>(spriteId);
                }
            }
            return nullptr;
        }

        std::vector<std::shared_ptr<ScThing>> getAllThings(const std::string& type)
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
                targetList = SPRITE_LIST_VEHICLE;
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
                duk_error(_context, DUK_ERR_ERROR, "Invalid thing type.");
            }

            std::vector<std::shared_ptr<ScThing>> result;
            auto spriteId = gSpriteListHead[targetList];
            while (spriteId != SPRITE_INDEX_NULL)
            {
                auto sprite = get_sprite(spriteId);
                if (sprite != nullptr)
                {
                    // Only the misc list checks the type property
                    if (targetList != SPRITE_LIST_MISC || sprite->generic.type == targetType)
                    {
                        result.push_back(std::make_shared<ScThing>(spriteId));
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
            dukglue_register_property(ctx, &ScMap::numThings_get, nullptr, "numThings");
            dukglue_register_property(ctx, &ScMap::rides_get, nullptr, "rides");
            dukglue_register_method(ctx, &ScMap::getRide, "getRide");
            dukglue_register_method(ctx, &ScMap::getTile, "getTile");
            dukglue_register_method(ctx, &ScMap::getThing, "getThing");
            dukglue_register_method(ctx, &ScMap::getAllThings, "getAllThings");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
