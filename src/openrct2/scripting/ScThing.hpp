#pragma once

#include <dukglue/dukglue.h>
#include "../common.h"
#include "../world/Sprite.h"

namespace OpenRCT2::Scripting
{
    class ScThing
    {
    private:
        rct_sprite * _sprite;

    public:
        ScThing(rct_sprite * sprite)
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

        sint32 x_get() { return _sprite->unknown.x; }
        sint32 y_get() { return _sprite->unknown.y; }
        sint32 z_get() { return _sprite->unknown.z; }

        uint8 tshirtColour_get() { return _sprite->peep.tshirt_colour; }
        void tshirtColour_set(uint8 value) { _sprite->peep.tshirt_colour = value; }
        uint8 trousersColour_get() { return _sprite->peep.trousers_colour; }
        void trousersColour_set(uint8 value) { _sprite->peep.trousers_colour = value; }

        static void Register(duk_context * ctx)
        {
            dukglue_register_property(ctx, &ScThing::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScThing::x_get, nullptr, "x");
            dukglue_register_property(ctx, &ScThing::y_get, nullptr, "y");
            dukglue_register_property(ctx, &ScThing::z_get, nullptr, "z");
            dukglue_register_property(ctx, &ScThing::tshirtColour_get, &ScThing::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScThing::trousersColour_get, &ScThing::trousersColour_set, "trousersColour");
        }
    };
}
