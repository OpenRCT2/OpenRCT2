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
#    include "../world/Sprite.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

namespace OpenRCT2::Scripting
{
    class ScThing
    {
    private:
        uint16_t _id = SPRITE_INDEX_NULL;

    public:
        ScThing(uint16_t id)
            : _id(id)
        {
        }

    private:
        std::string type_get()
        {
            auto thing = GetThing();
            if (thing != nullptr)
            {
                switch (thing->sprite_identifier)
                {
                    case SPRITE_IDENTIFIER_VEHICLE:
                        return "car";
                    case SPRITE_IDENTIFIER_PEEP:
                        return "peep";
                    case SPRITE_IDENTIFIER_MISC:
                        switch (thing->type)
                        {
                            case SPRITE_MISC_BALLOON:
                                return "balloon";
                            case SPRITE_MISC_DUCK:
                                return "duck";
                        }
                        break;
                    case SPRITE_IDENTIFIER_LITTER:
                        return "litter";
                }
            }
            return "unknown";
        }

        // x getter and setter
        int32_t x_get()
        {
            auto thing = GetThing();
            return thing != nullptr ? thing->x : 0;
        }
        void x_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto thing = GetThing();
            if (thing != nullptr)
            {
                sprite_move(value, thing->y, thing->z, thing);
            }
        }

        // y getter and setter
        int32_t y_get()
        {
            auto thing = GetThing();
            return thing != nullptr ? thing->y : 0;
        }
        void y_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto thing = GetThing();
            if (thing != nullptr)
            {
                sprite_move(thing->x, value, thing->z, thing);
            }
        }

        // z getter and setter
        int16_t z_get()
        {
            auto thing = GetThing();
            return thing != nullptr ? thing->z : 0;
        }
        void z_set(int16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto thing = GetThing();
            if (thing != nullptr)
            {
                sprite_move(thing->x, thing->y, value, thing);
            }
        }

        uint8_t tshirtColour_get()
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->tshirt_colour : 0;
        }
        void tshirtColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->tshirt_colour = value;
            }
        }
        uint8_t trousersColour_get()
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->trousers_colour : 0;
        }
        void trousersColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->trousers_colour = value;
            }
        }

        SpriteBase* GetThing()
        {
            return &get_sprite(_id)->generic;
        }

        Peep* GetPeep()
        {
            return get_sprite(_id)->AsPeep();
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScThing::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScThing::x_get, &ScThing::x_set, "x");
            dukglue_register_property(ctx, &ScThing::y_get, &ScThing::y_set, "y");
            dukglue_register_property(ctx, &ScThing::z_get, &ScThing::z_set, "z");
            dukglue_register_property(ctx, &ScThing::tshirtColour_get, &ScThing::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScThing::trousersColour_get, &ScThing::trousersColour_set, "trousersColour");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
