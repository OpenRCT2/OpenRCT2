/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScGuest.hpp"

#    include "../../../entity/Guest.h"
#    include "../../../localisation/Localisation.h"

namespace OpenRCT2::Scripting
{
    ScGuest::ScGuest(EntityId id)
        : ScPeep(id)
    {
    }

    void ScGuest::Register(duk_context* ctx)
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
        dukglue_register_property(ctx, &ScGuest::isInPark_get, nullptr, "isInPark");
        dukglue_register_property(ctx, &ScGuest::isLost_get, nullptr, "isLost");
        dukglue_register_property(ctx, &ScGuest::lostCountdown_get, &ScGuest::lostCountdown_set, "lostCountdown");
        dukglue_register_property(ctx, &ScGuest::thoughts_get, nullptr, "thoughts");
    }

    Guest* ScGuest::GetGuest() const
    {
        return ::GetEntity<Guest>(_id);
    }

    uint8_t ScGuest::tshirtColour_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->TshirtColour : 0;
    }
    void ScGuest::tshirtColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->TshirtColour = value;
            peep->Invalidate();
        }
    }

    uint8_t ScGuest::trousersColour_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->TrousersColour : 0;
    }
    void ScGuest::trousersColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->TrousersColour = value;
            peep->Invalidate();
        }
    }

    uint8_t ScGuest::balloonColour_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->BalloonColour : 0;
    }
    void ScGuest::balloonColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->BalloonColour = value;
            peep->Invalidate();
        }
    }

    uint8_t ScGuest::hatColour_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->HatColour : 0;
    }
    void ScGuest::hatColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->HatColour = value;
            peep->Invalidate();
        }
    }

    uint8_t ScGuest::umbrellaColour_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->UmbrellaColour : 0;
    }
    void ScGuest::umbrellaColour_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->UmbrellaColour = value;
            peep->Invalidate();
        }
    }

    uint8_t ScGuest::happiness_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Happiness : 0;
    }
    void ScGuest::happiness_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Happiness = value;
        }
    }

    uint8_t ScGuest::happinessTarget_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->HappinessTarget : 0;
    }
    void ScGuest::happinessTarget_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->HappinessTarget = value;
        }
    }

    uint8_t ScGuest::nausea_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Nausea : 0;
    }
    void ScGuest::nausea_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Nausea = value;
        }
    }

    uint8_t ScGuest::nauseaTarget_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->NauseaTarget : 0;
    }
    void ScGuest::nauseaTarget_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->NauseaTarget = value;
        }
    }

    uint8_t ScGuest::hunger_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Hunger : 0;
    }
    void ScGuest::hunger_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Hunger = value;
        }
    }

    uint8_t ScGuest::thirst_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Thirst : 0;
    }
    void ScGuest::thirst_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Thirst = value;
        }
    }

    uint8_t ScGuest::toilet_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Toilet : 0;
    }
    void ScGuest::toilet_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Toilet = value;
        }
    }

    uint8_t ScGuest::mass_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Mass : 0;
    }
    void ScGuest::mass_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Mass = value;
        }
    }

    uint8_t ScGuest::minIntensity_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Intensity.GetMinimum() : 0;
    }
    void ScGuest::minIntensity_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Intensity = peep->Intensity.WithMinimum(value);
        }
    }

    uint8_t ScGuest::maxIntensity_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->Intensity.GetMaximum() : 0;
    }
    void ScGuest::maxIntensity_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->Intensity = peep->Intensity.WithMaximum(value);
        }
    }

    uint8_t ScGuest::nauseaTolerance_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? EnumValue(peep->NauseaTolerance) : 0;
    }
    void ScGuest::nauseaTolerance_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->NauseaTolerance = static_cast<PeepNauseaTolerance>(std::min<uint8_t>(value, 3));
        }
    }

    int32_t ScGuest::cash_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->CashInPocket : 0;
    }
    void ScGuest::cash_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->CashInPocket = std::max(0, value);
        }
    }

    bool ScGuest::isInPark_get() const
    {
        auto peep = GetGuest();
        return (peep != nullptr && !peep->OutsideOfPark);
    }

    bool ScGuest::isLost_get() const
    {
        auto peep = GetGuest();
        return (peep != nullptr && peep->GuestIsLostCountdown < 90);
    }

    uint8_t ScGuest::lostCountdown_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? peep->GuestIsLostCountdown : 0;
    }
    void ScGuest::lostCountdown_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->GuestIsLostCountdown = value;
        }
    }

    DukValue ScGuest::thoughts_get() const
    {
        auto ctx = GetContext()->GetScriptEngine().GetContext();

        duk_push_array(ctx);

        auto peep = GetGuest();
        if (peep != nullptr)
        {
            duk_uarridx_t index = 0;
            for (const auto& thought : peep->Thoughts)
            {
                if (thought.type == PeepThoughtType::None)
                    break;
                if (thought.freshness == 0)
                    continue;
                auto scThoughtPtr = std::make_shared<ScThought>(thought);
                auto dukThought = GetObjectAsDukValue(ctx, scThoughtPtr);
                dukThought.push();
                duk_put_prop_index(ctx, -2, index);
                index++;
            }
        }

        return DukValue::take_from_stack(ctx, -1);
    }

    ScThought::ScThought(PeepThought backing)
        : _backing(backing)
    {
    }

    void ScThought::Register(duk_context *ctx)
    {
        dukglue_register_property(ctx, &ScThought::type_get, nullptr, "type");
        dukglue_register_property(ctx, &ScThought::item_get, nullptr, "item");
        dukglue_register_property(ctx, &ScThought::freshness_get, nullptr, "freshness");
        dukglue_register_property(ctx, &ScThought::freshTimeout_get, nullptr, "freshTimeout");
        dukglue_register_method(ctx, &ScThought::toString, "toString");
    }

    uint8_t ScThought::type_get() const
    {
        return EnumValue(_backing.type);
    }

    uint16_t ScThought::item_get() const
    {
        return _backing.item;
    }

    uint8_t ScThought::freshness_get() const
    {
        return _backing.freshness;
    }

    uint8_t ScThought::freshTimeout_get() const
    {
        return _backing.fresh_timeout;
    }

    std::string ScThought::toString() const
    {
        // format string with arguments
        auto ft = Formatter();
        peep_thought_set_format_args(&_backing, ft);
        return format_string(STR_STRINGID, ft.Data());
    }

} // namespace OpenRCT2::Scripting

#endif
