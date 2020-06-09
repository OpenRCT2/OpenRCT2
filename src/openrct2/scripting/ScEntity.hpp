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

#    include "../Context.h"
#    include "../common.h"
#    include "../peep/Peep.h"
#    include "../peep/Staff.h"
#    include "../world/Sprite.h"
#    include "Duktape.hpp"
#    include "ScriptEngine.h"

#    include <algorithm>
#    include <string_view>
#    include <unordered_map>

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
                invalidate_sprite_2(entity);
                entity->MoveTo({ value, entity->y, entity->z });
                invalidate_sprite_2(entity);
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
                invalidate_sprite_2(entity);
                entity->MoveTo({ entity->x, value, entity->z });
                invalidate_sprite_2(entity);
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
                invalidate_sprite_2(entity);
                entity->MoveTo({ entity->x, entity->y, value });
                invalidate_sprite_2(entity);
            }
        }

        void remove()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto entity = GetEntity();
            if (entity != nullptr)
            {
                invalidate_sprite_2(entity);
                switch (entity->sprite_identifier)
                {
                    case SPRITE_IDENTIFIER_VEHICLE:
                        duk_error(ctx, DUK_ERR_ERROR, "Removing a vehicle is currently unsupported.");
                        break;
                    case SPRITE_IDENTIFIER_PEEP:
                    {
                        auto peep = static_cast<Peep*>(entity);
                        // We can't remove a single peep from a ride at the moment as this can cause complications with the
                        // vehicle car having an unsupported peep capacity.
                        if (peep->state == PEEP_STATE_ON_RIDE || peep->state == PEEP_STATE_ENTERING_RIDE)
                        {
                            duk_error(ctx, DUK_ERR_ERROR, "Removing a peep that is on a ride is currently unsupported.");
                        }
                        else
                        {
                            peep->Remove();
                        }
                        break;
                    }
                    case SPRITE_IDENTIFIER_MISC:
                    case SPRITE_IDENTIFIER_LITTER:
                        sprite_remove(entity);
                        break;
                }
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
            dukglue_register_method(ctx, &ScEntity::remove, "remove");
        }
    };

    static const DukEnumMap<uint32_t> PeepFlagMap({
        { "leavingPark", PEEP_FLAGS_LEAVING_PARK },
        { "slowWalk", PEEP_FLAGS_SLOW_WALK },
        { "tracking", PEEP_FLAGS_TRACKING },
        { "waving", PEEP_FLAGS_WAVING },
        { "hasPaidForParkEntry", PEEP_FLAGS_HAS_PAID_FOR_PARK_ENTRY },
        { "photo", PEEP_FLAGS_PHOTO },
        { "painting", PEEP_FLAGS_PAINTING },
        { "wow", PEEP_FLAGS_WOW },
        { "litter", PEEP_FLAGS_LITTER },
        { "lost", PEEP_FLAGS_LOST },
        { "hunger", PEEP_FLAGS_HUNGER },
        { "toilet", PEEP_FLAGS_TOILET },
        { "crowded", PEEP_FLAGS_CROWDED },
        { "happiness", PEEP_FLAGS_HAPPINESS },
        { "nausea", PEEP_FLAGS_NAUSEA },
        { "purple", PEEP_FLAGS_PURPLE },
        { "pizza", PEEP_FLAGS_PIZZA },
        { "explode", PEEP_FLAGS_EXPLODE },
        { "rideShouldBeMarkedAsFavourite", PEEP_FLAGS_RIDE_SHOULD_BE_MARKED_AS_FAVOURITE },
        { "parkEntranceChosen", PEEP_FLAGS_PARK_ENTRANCE_CHOSEN },
        { "contagious", PEEP_FLAGS_CONTAGIOUS },
        { "joy", PEEP_FLAGS_JOY },
        { "angry", PEEP_FLAGS_ANGRY },
        { "iceCream", PEEP_FLAGS_ICE_CREAM },
        { "hereWeAre", PEEP_FLAGS_HERE_WE_ARE },
    });

    class ScPeep : public ScEntity
    {
    public:
        ScPeep(uint16_t id)
            : ScEntity(id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScEntity, ScPeep>(ctx);
            dukglue_register_property(ctx, &ScPeep::peepType_get, nullptr, "peepType");
            dukglue_register_property(ctx, &ScPeep::name_get, &ScPeep::name_set, "name");
            dukglue_register_property(ctx, &ScPeep::destination_get, &ScPeep::destination_set, "destination");
            dukglue_register_property(ctx, &ScPeep::energy_get, &ScPeep::energy_set, "energy");
            dukglue_register_property(ctx, &ScPeep::energyTarget_get, &ScPeep::energyTarget_set, "energyTarget");
            dukglue_register_method(ctx, &ScPeep::getFlag, "getFlag");
            dukglue_register_method(ctx, &ScPeep::setFlag, "setFlag");
        }

    private:
        std::string peepType_get() const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return peep->type == PEEP_TYPE_STAFF ? "staff" : "guest";
            }
            return "";
        }

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

        bool getFlag(const std::string& key) const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto mask = PeepFlagMap[key];
                return (peep->PeepFlags & mask) != 0;
            }
            return false;
        }

        void setFlag(const std::string& key, bool value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto mask = PeepFlagMap[key];
                if (value)
                    peep->PeepFlags |= mask;
                else
                    peep->PeepFlags &= ~mask;
                peep->Invalidate();
            }
        }

        DukValue destination_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return ToDuk(ctx, CoordsXY(peep->destination_x, peep->destination_y));
            }
            return ToDuk(ctx, nullptr);
        }

        void destination_set(const DukValue& value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                auto pos = FromDuk<CoordsXY>(value);
                peep->destination_x = pos.x;
                peep->destination_y = pos.y;
                peep->Invalidate();
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

    protected:
        Peep* GetPeep() const
        {
            return get_sprite(_id)->generic.As<Peep>();
        }
    };

    class ScGuest : public ScPeep
    {
    public:
        ScGuest(uint16_t id)
            : ScPeep(id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScPeep, ScGuest>(ctx);
            dukglue_register_property(ctx, &ScGuest::tshirtColour_get, &ScGuest::tshirtColour_set, "tshirtColour");
            dukglue_register_property(ctx, &ScGuest::trousersColour_get, &ScGuest::trousersColour_set, "trousersColour");
            dukglue_register_property(ctx, &ScGuest::balloonColour_get, &ScGuest::balloonColour_set, "balloonColour");
            dukglue_register_property(ctx, &ScGuest::hatColour_get, &ScGuest::hatColour_set, "hatColour");
            dukglue_register_property(ctx, &ScGuest::umbrellaColour_get, &ScGuest::umbrellaColour_set, "umbrellaColour");
            dukglue_register_property(ctx, &ScGuest::happiness_get, &ScGuest::happiness_set, "happiness");
            dukglue_register_property(ctx, &ScGuest::happinessTarget_get, &ScGuest::happinessTarget_set, "happinessTarget");
            dukglue_register_property(ctx, &ScGuest::nausea_get, &ScGuest::nausea_set, "nausea");
            dukglue_register_property(ctx, &ScGuest::nauseaTarget_get, &ScGuest::nauseaTarget_set, "nauseaTarget");
            dukglue_register_property(ctx, &ScGuest::hunger_get, &ScGuest::hunger_set, "hunger");
            dukglue_register_property(ctx, &ScGuest::thirst_get, &ScGuest::thirst_set, "thirst");
            dukglue_register_property(ctx, &ScGuest::toilet_get, &ScGuest::toilet_set, "toilet");
            dukglue_register_property(ctx, &ScGuest::mass_get, &ScGuest::mass_set, "mass");
            dukglue_register_property(ctx, &ScGuest::minIntensity_get, &ScGuest::minIntensity_set, "minIntensity");
            dukglue_register_property(ctx, &ScGuest::maxIntensity_get, &ScGuest::maxIntensity_set, "maxIntensity");
            dukglue_register_property(ctx, &ScGuest::nauseaTolerance_get, &ScGuest::nauseaTolerance_set, "nauseaTolerance");
            dukglue_register_property(ctx, &ScGuest::cash_get, &ScGuest::cash_set, "cash");
        }

    private:
        Guest* GetGuest() const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return peep->AsGuest();
            }
            return nullptr;
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
                peep->Invalidate();
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
                peep->Invalidate();
            }
        }

        uint8_t balloonColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->BalloonColour : 0;
        }
        void balloonColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->BalloonColour = value;
                peep->Invalidate();
            }
        }

        uint8_t hatColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->HatColour : 0;
        }
        void hatColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->HatColour = value;
                peep->Invalidate();
            }
        }

        uint8_t umbrellaColour_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->UmbrellaColour : 0;
        }
        void umbrellaColour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->UmbrellaColour = value;
                peep->Invalidate();
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
            return peep != nullptr ? peep->Nausea : 0;
        }
        void nausea_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Nausea = value;
            }
        }

        uint8_t nauseaTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->NauseaTarget : 0;
        }
        void nauseaTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->NauseaTarget = value;
            }
        }

        uint8_t hunger_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Hunger : 0;
        }
        void hunger_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Hunger = value;
            }
        }

        uint8_t thirst_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Thirst : 0;
        }
        void thirst_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Thirst = value;
            }
        }

        uint8_t toilet_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Toilet : 0;
        }
        void toilet_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Toilet = value;
            }
        }

        uint8_t mass_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Mass : 0;
        }
        void mass_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Mass = value;
            }
        }

        uint8_t minIntensity_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Intensity.GetMinimum() : 0;
        }
        void minIntensity_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Intensity = peep->Intensity.WithMinimum(value);
            }
        }

        uint8_t maxIntensity_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Intensity.GetMaximum() : 0;
        }
        void maxIntensity_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Intensity = peep->Intensity.WithMaximum(value);
            }
        }

        uint8_t nauseaTolerance_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->NauseaTolerance : 0;
        }
        void nauseaTolerance_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->NauseaTolerance = std::min<uint8_t>(value, 3);
            }
        }

        int32_t cash_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->CashInPocket : 0;
        }
        void cash_set(int32_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->CashInPocket = std::max(0, value);
            }
        }
    };

    class ScStaff : public ScPeep
    {
    public:
        ScStaff(uint16_t Id)
            : ScPeep(Id)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScPeep, ScStaff>(ctx);
            dukglue_register_property(ctx, &ScStaff::staffType_get, &ScStaff::staffType_set, "staffType");
            dukglue_register_property(ctx, &ScStaff::colour_get, &ScStaff::colour_set, "colour");
            dukglue_register_property(ctx, &ScStaff::costume_get, &ScStaff::costume_set, "costume");
            dukglue_register_property(ctx, &ScStaff::orders_get, &ScStaff::orders_set, "orders");
        }

    private:
        Staff* GetStaff() const
        {
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                return peep->AsStaff();
            }
            return nullptr;
        }

        std::string staffType_get() const
        {
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                switch (peep->staff_type)
                {
                    case STAFF_TYPE_HANDYMAN:
                        return "handyman";
                    case STAFF_TYPE_MECHANIC:
                        return "mechanic";
                    case STAFF_TYPE_SECURITY:
                        return "security";
                    case STAFF_TYPE_ENTERTAINER:
                        return "entertainer";
                }
            }
            return "";
        }

        void staffType_set(const std::string& value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                if (value == "handyman" && peep->staff_type != STAFF_TYPE_HANDYMAN)
                {
                    peep->staff_type = STAFF_TYPE_HANDYMAN;
                    peep->sprite_type = PeepSpriteType::PEEP_SPRITE_TYPE_HANDYMAN;
                }
                else if (value == "mechanic" && peep->staff_type != STAFF_TYPE_MECHANIC)
                {
                    peep->staff_type = STAFF_TYPE_MECHANIC;
                    peep->sprite_type = PeepSpriteType::PEEP_SPRITE_TYPE_MECHANIC;
                }
                else if (value == "security" && peep->staff_type != STAFF_TYPE_SECURITY)
                {
                    peep->staff_type = STAFF_TYPE_SECURITY;
                    peep->sprite_type = PeepSpriteType::PEEP_SPRITE_TYPE_SECURITY;
                }
                else if (value == "entertainer" && peep->staff_type != STAFF_TYPE_ENTERTAINER)
                {
                    peep->staff_type = STAFF_TYPE_ENTERTAINER;
                    peep->sprite_type = PeepSpriteType::PEEP_SPRITE_TYPE_ENTERTAINER_PANDA;
                }
            }
        }

        uint8_t colour_get() const
        {
            auto peep = GetStaff();
            return peep != nullptr ? peep->tshirt_colour : 0;
        }

        void colour_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                peep->tshirt_colour = value;
                peep->trousers_colour = value;
            }
        }

        uint8_t costume_get() const
        {
            auto peep = GetStaff();
            if (peep != nullptr && peep->staff_type == STAFF_TYPE_ENTERTAINER)
            {
                return peep->GetCostume();
            }
            return 0;
        }

        void costume_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                peep->SetCostume(value);
            }
        }

        uint8_t orders_get() const
        {
            auto peep = GetStaff();
            return peep != nullptr ? peep->StaffOrders : 0;
        }

        void orders_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetStaff();
            if (peep != nullptr)
            {
                peep->StaffOrders = value;
            }
        }
    };

} // namespace OpenRCT2::Scripting

#endif
