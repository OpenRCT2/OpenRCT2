/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../ride/Ride.h"
#include "../world/Map.h"
#include "Duktape.hpp"
#include "ScThing.hpp"
#include "ScTile.hpp"

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

        int32_t rides_get()
        {
            return MAX_RIDES;
        }

        int32_t things_get()
        {
            return MAX_SPRITES;
        }

        std::shared_ptr<ScTile> getTile(int32_t x, int32_t y)
        {
            auto firstElement = map_get_first_element_at({ x, y });
            return std::make_shared<ScTile>(firstElement);
        }

        std::shared_ptr<ScThing> getThing(int32_t id)
        {
            if (id >= 0 && id < MAX_SPRITES)
            {
                auto sprite = get_sprite(id);
                if (sprite != nullptr && sprite->generic.sprite_identifier != SPRITE_IDENTIFIER_NULL)
                {
                    return std::make_shared<ScThing>(sprite);
                }
            }
            return nullptr;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScMap::size_get, nullptr, "size");
            dukglue_register_property(ctx, &ScMap::rides_get, nullptr, "rides");
            dukglue_register_property(ctx, &ScMap::things_get, nullptr, "things");
            dukglue_register_method(ctx, &ScMap::getTile, "getTile");
            dukglue_register_method(ctx, &ScMap::getThing, "getThing");
        }
    };
} // namespace OpenRCT2::Scripting
