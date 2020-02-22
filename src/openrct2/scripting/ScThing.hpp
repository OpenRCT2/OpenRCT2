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
#include "Duktape.hpp"

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
            if (_sprite->generic.sprite_identifier == SPRITE_IDENTIFIER_PEEP)
            {
                return "peep";
            }
            return "unknown";
        }

        // x getter and setter
        int32_t x_get()
        {
            return _sprite->generic.x;
        }
        void x_set(int32_t value)
        {
            _sprite->generic.x = value;
        }

        // y getter and setter
        int32_t y_get()
        {
            return _sprite->generic.y;
        }
        void y_set(int32_t value)
        {
            _sprite->generic.y = value;
        }

        // z getter and setter
        int16_t z_get()
        {
            return _sprite->generic.z;
        }
        void z_set(int16_t value)
        {
            _sprite->generic.z = value;
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

        template<typename T> static void dukglue_property_helper(duk_context* ctx, T& var, const char* name)
        {
            auto getter = []() -> const T { return var; };
            auto setter = [&var](T& value) -> void { var = value; };
            dukglue_register_property(ctx, getter, setter, name);
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_constructor<ScThing, rct_sprite*>(ctx, "Thing");
            dukglue_register_property(ctx, &ScThing::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScThing::x_get, &ScThing::x_set, "x");
            dukglue_register_property(ctx, &ScThing::y_get, &ScThing::y_set, "y");
            dukglue_register_property(ctx, &ScThing::z_get, &ScThing::z_set, "z");
            dukglue_register_property(ctx, &ScThing::tshirtColour_get, &ScThing::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScThing::trousersColour_get, &ScThing::trousersColour_set, "trousersColour");
        }
    };
} // namespace OpenRCT2::Scripting
