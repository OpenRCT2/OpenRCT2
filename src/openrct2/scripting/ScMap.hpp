#pragma once

#include <dukglue/dukglue.h>
#include "../common.h"
#include "../ride/Ride.h"
#include "../world/Map.h"
#include "ScTile.hpp"
#include "ScThing.hpp"

namespace OpenRCT2::Scripting
{
    class ScMap
    {
    private:
        duk_context * _context;

    public:
        ScMap(duk_context * ctx)
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

        sint32 rides_get()
        {
            return MAX_RIDES;
        }

        sint32 things_get()
        {
            return MAX_SPRITES;
        }

        std::shared_ptr<ScTile> getTile(sint32 x, sint32 y)
        {
            auto firstElement = map_get_first_element_at(x, y);
            return std::make_shared<ScTile>(firstElement);
        }

        std::shared_ptr<ScThing> getThing(sint32 id)
        {
            if (id >= 0 && id < MAX_SPRITES)
            {
                auto sprite = get_sprite(id);
                if (sprite != nullptr && sprite->unknown.sprite_identifier != SPRITE_IDENTIFIER_NULL)
                {
                    return std::make_shared<ScThing>(sprite);
                }
            }
            return nullptr;
        }

        static void Register(duk_context * ctx)
        {
            dukglue_register_property(ctx, &ScMap::size_get, nullptr, "size");
            dukglue_register_property(ctx, &ScMap::rides_get, nullptr, "rides");
            dukglue_register_property(ctx, &ScMap::things_get, nullptr, "things");
            dukglue_register_method(ctx, &ScMap::getTile, "getTile");
            dukglue_register_method(ctx, &ScMap::getThing, "getThing");
        }
    };
}
