/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "ScEntity.hpp"

namespace OpenRCT2::Scripting
{
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
        ScPeep(EntityId id)
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
                return peep->Is<Staff>() ? "staff" : "guest";
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
                return ToDuk(ctx, peep->GetDestination());
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
                peep->SetDestination(pos);
                peep->Invalidate();
            }
        }

        uint8_t energy_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->Energy : 0;
        }
        void energy_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->Energy = value;
            }
        }

        uint8_t energyTarget_get() const
        {
            auto peep = GetPeep();
            return peep != nullptr ? peep->EnergyTarget : 0;
        }
        void energyTarget_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetPeep();
            if (peep != nullptr)
            {
                peep->EnergyTarget = value;
            }
        }

    protected:
        Peep* GetPeep() const
        {
            return ::GetEntity<Peep>(_id);
        }
    };

} // namespace OpenRCT2::Scripting

#endif
