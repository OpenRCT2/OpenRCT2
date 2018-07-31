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
#include "../world/Sprite.h"

#include <dukglue/dukglue.h>

namespace OpenRCT2::Scripting
{
    class ScThing
    {
    private:
        rct_sprite* _sprite;

    public:
        ScThing(rct_sprite* sprite)
            : _sprite(sprite)
        {
        }

        std::string type_get()
        {
            if (_sprite->unknown.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
            {
                return "peep";
            }
            return "unknown";
        }

        int32_t x_get()
        {
            return _sprite->unknown.x;
        }
        int32_t y_get()
        {
            return _sprite->unknown.y;
        }
        int32_t z_get()
        {
            return _sprite->unknown.z;
        }

        uint8_t tshirtColour_get()
        {
            return _sprite->peep.tshirt_colour;
        }
        void tshirtColour_set(uint8_t value)
        {
            _sprite->peep.tshirt_colour = value;
        }
        uint8_t trousersColour_get()
        {
            return _sprite->peep.trousers_colour;
        }
        void trousersColour_set(uint8_t value)
        {
            _sprite->peep.trousers_colour = value;
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScThing::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScThing::x_get, nullptr, "x");
            dukglue_register_property(ctx, &ScThing::y_get, nullptr, "y");
            dukglue_register_property(ctx, &ScThing::z_get, nullptr, "z");
            dukglue_register_property(ctx, &ScThing::tshirtColour_get, &ScThing::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScThing::trousersColour_get, &ScThing::trousersColour_set, "trousersColour");
        }
    };
} // namespace OpenRCT2::Scripting
