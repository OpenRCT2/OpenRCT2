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
#    include "../world/Sprite.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>

namespace OpenRCT2::Scripting
{
    class ScEntity
    {
    protected:
        uint16_t _id = SPRITE_INDEX_NULL;

    public:
        ScEntity(uint16_t id)
            : _id(id)
        {
        }

    private:
        int32_t id_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->sprite_index : 0;
        }

        std::string type_get() const
        {
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                switch (entity->sprite_identifier)
                {
                    case SPRITE_IDENTIFIER_VEHICLE:
                        return "car";
                    case SPRITE_IDENTIFIER_PEEP:
                        return "peep";
                    case SPRITE_IDENTIFIER_MISC:
                        switch (entity->type)
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
        int32_t x_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->x : 0;
        }
        void x_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                sprite_move(value, entity->y, entity->z, entity);
            }
        }

        // y getter and setter
        int32_t y_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->y : 0;
        }
        void y_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                sprite_move(entity->x, value, entity->z, entity);
            }
        }

        // z getter and setter
        int16_t z_get() const
        {
            auto entity = GetEntity();
            return entity != nullptr ? entity->z : 0;
        }
        void z_set(int16_t value)
        {
            ThrowIfGameStateNotMutable();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                sprite_move(entity->x, entity->y, value, entity);
            }
        }

        SpriteBase* GetEntity() const
        {
            return &get_sprite(_id)->generic;
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScEntity::id_get, nullptr, "id");
            dukglue_register_property(ctx, &ScEntity::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScEntity::x_get, &ScEntity::x_set, "x");
            dukglue_register_property(ctx, &ScEntity::y_get, &ScEntity::y_set, "y");
            dukglue_register_property(ctx, &ScEntity::z_get, &ScEntity::z_set, "z");
        }
    };

    class ScPeep : public ScEntity
    {
    public:
        ScPeep(uint16_t id)
            : ScEntity(id)
        {
        }

    private:
        std::string name_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->GetName() : std::string();
        }
        void name_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->SetName(value);
            }
        }

        uint8_t tshirtColour_get() const
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

        uint8_t trousersColour_get() const
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

        uint8_t energy_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->energy : 0;
        }
        void energy_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->energy = value;
            }
        }

        uint8_t energyTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->energy_target : 0;
        }
        void energyTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->energy_target = value;
            }
        }

        uint8_t happiness_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->happiness : 0;
        }
        void happiness_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->happiness = value;
            }
        }

        uint8_t happinessTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->happiness_target : 0;
        }
        void happinessTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->happiness_target = value;
            }
        }

        uint8_t nausea_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->nausea : 0;
        }
        void nausea_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->nausea = value;
            }
        }

        uint8_t nauseaTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->nausea_target : 0;
        }
        void nauseaTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->nausea_target = value;
            }
        }

        uint8_t hunger_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->hunger : 0;
        }
        void hunger_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->hunger = value;
            }
        }

        uint8_t thirst_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->thirst : 0;
        }
        void thirst_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->thirst = value;
            }
        }

        uint8_t toilet_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->toilet : 0;
        }
        void toilet_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->toilet = value;
            }
        }

        uint8_t mass_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->mass : 0;
        }
        void mass_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->mass = value;
            }
        }

        uint8_t minIntensity_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->intensity & 0x0F : 0;
        }
        void minIntensity_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto minIntensity = std::min<uint8_t>(value, 15);
                auto maxIntensity = peep->intensity >> 4;
                peep->intensity = (maxIntensity << 4) | minIntensity;
            }
        }

        uint8_t maxIntensity_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->intensity >> 4 : 0;
        }
        void maxIntensity_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto minIntensity = peep->intensity & 0x0F;
                auto maxIntensity = std::min<uint8_t>(value, 15);
                peep->intensity = (maxIntensity << 4) | minIntensity;
            }
        }

        uint8_t nauseaTolerance_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->nausea_tolerance : 0;
        }
        void nauseaTolerance_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->nausea_tolerance = std::min<uint8_t>(value, 3);
            }
        }

        int32_t cash_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->cash_in_pocket : 0;
        }
        void cash_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->cash_in_pocket = std::max(0, value);
            }
        }

        Peep* GetPeep() const
        {
            return get_sprite(_id)->AsPeep();
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScEntity, ScPeep>(ctx);
            dukglue_register_property(ctx, &ScPeep::name_get, &ScPeep::name_set, "name");
            dukglue_register_property(ctx, &ScPeep::tshirtColour_get, &ScPeep::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScPeep::trousersColour_get, &ScPeep::trousersColour_set, "trousersColour");
            dukglue_register_property(ctx, &ScPeep::energy_get, &ScPeep::energy_set, "energy");
            dukglue_register_property(ctx, &ScPeep::energyTarget_get, &ScPeep::energyTarget_set, "energyTarget");
            dukglue_register_property(ctx, &ScPeep::happiness_get, &ScPeep::happiness_set, "happiness");
            dukglue_register_property(ctx, &ScPeep::happinessTarget_get, &ScPeep::happinessTarget_set, "happinessTarget");
            dukglue_register_property(ctx, &ScPeep::nausea_get, &ScPeep::nausea_set, "nausea");
            dukglue_register_property(ctx, &ScPeep::nauseaTarget_get, &ScPeep::nauseaTarget_set, "nauseaTarget");
            dukglue_register_property(ctx, &ScPeep::hunger_get, &ScPeep::hunger_set, "hunger");
            dukglue_register_property(ctx, &ScPeep::thirst_get, &ScPeep::thirst_set, "thirst");
            dukglue_register_property(ctx, &ScPeep::toilet_get, &ScPeep::toilet_set, "toilet");
            dukglue_register_property(ctx, &ScPeep::mass_get, &ScPeep::mass_set, "mass");
            dukglue_register_property(ctx, &ScPeep::minIntensity_get, &ScPeep::minIntensity_set, "minIntensity");
            dukglue_register_property(ctx, &ScPeep::maxIntensity_get, &ScPeep::maxIntensity_set, "maxIntensity");
            dukglue_register_property(ctx, &ScPeep::nauseaTolerance_get, &ScPeep::nauseaTolerance_set, "nauseaTolerance");
            dukglue_register_property(ctx, &ScPeep::cash_get, &ScPeep::cash_set, "cash");
        }
    };

} // namespace OpenRCT2::Scripting

#endif
