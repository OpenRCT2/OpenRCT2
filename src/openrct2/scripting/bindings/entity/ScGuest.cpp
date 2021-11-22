/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScGuest.hpp"
#    include "../../../peep/Guest.h"
#    include "../../../peep/Peep.h"
#    include "../../../ride/Ride.h"
#    include "../../../ride/ShopItem.h"
#    include "../../../management/Marketing.h"

namespace OpenRCT2::Scripting
{
    ScGuest::ScGuest(uint16_t id)
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
        dukglue_register_property(ctx, &ScGuest::headingToRideId_get, &ScGuest::headingToRideId_set, "headingToRideId");
        dukglue_register_property(ctx, &ScGuest::inventory_get, nullptr, "inventory");
        dukglue_register_method(ctx, &ScGuest::giveItem, "giveItem");
        dukglue_register_method(ctx, &ScGuest::removeItem, "removeItem");
        dukglue_register_method(ctx, &ScGuest::removeAllItems, "removeAllItems");
        dukglue_register_property(ctx, &ScGuest::voucherType_get, &ScGuest::voucherType_set, "voucherType");
        dukglue_register_property(ctx, &ScGuest::voucherId_get, &ScGuest::voucherId_set, "voucherId");
        dukglue_register_property(ctx, &ScGuest::previousRide_get, nullptr, "previousRide");
        dukglue_register_property(ctx, &ScGuest::currentRide_get, nullptr, "currentRide");
        dukglue_register_property(ctx, &ScGuest::currentRideStation_get, nullptr, "currentRideStation");
        dukglue_register_property(ctx, &ScGuest::interactionRide_get, nullptr, "interactionRide");
        dukglue_register_property(ctx, &ScGuest::peepState_get, nullptr, "peepState");
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

    int32_t ScGuest::headingToRideId_get() const
    {
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            return EnumValue(peep->GuestHeadingToRideId);
        }
        return EnumValue(RIDE_ID_NULL);
    }
    void ScGuest::headingToRideId_set(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->GuestHeadingToRideId = static_cast<ride_id_t>(value);
        }
    }

    std::vector<DukValue> ScGuest::inventory_get() const
    {
        auto peep = GetGuest();
        auto ctx = GetContext()->GetScriptEngine().GetContext();
        std::vector<DukValue> inventory;

        if (peep != nullptr)
        {
            for (int i = 0; i < 53; i++)
            {
                if (peep->HasItem(static_cast<ShopItem>(i)))
                    {
                        inventory.push_back(ToDuk<int32_t>(ctx, i));
                    };
            };
            
        };
        return inventory;
    } 

    void ScGuest::removeAllItems()
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
        {
            peep->RemoveAllItems();
        }
    }

    void ScGuest::giveItem(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
            {
                peep->GiveItem(static_cast<ShopItem>(value));
            }
    }

    void ScGuest::removeItem(int32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto peep = GetGuest();
        if (peep != nullptr)
            {
                peep->RemoveItem(static_cast<ShopItem>(value));
            }

    }

    uint8_t ScGuest::voucherType_get() const
    {
        auto peep = GetGuest();
        if (peep != nullptr)
            {
                switch (peep->VoucherType)
                    {
                        case VOUCHER_TYPE_PARK_ENTRY_FREE:
                            return 0;
                        case VOUCHER_TYPE_RIDE_FREE:
                            return 1;
                        case VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE:
                            return 2;
                        case VOUCHER_TYPE_FOOD_OR_DRINK_FREE:
                            return 3;
                    }
             }
        return 255;
    }   
    void ScGuest::voucherType_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetGuest();
            if (peep != nullptr)
                {
                    peep->VoucherType = value;
                }
        }

    uint8_t ScGuest::voucherId_get() const
        {
            auto peep = GetGuest();
            return peep != nullptr && static_cast<int>(peep->VoucherShopItem) > 0 ? static_cast<uint8_t>(peep->VoucherShopItem) : static_cast<uint8_t>(255);
        }
    void ScGuest::voucherId_set(uint8_t value)
        {
            ThrowIfGameStateNotMutable();
            auto peep = GetGuest();
            if (peep != nullptr)
                {
                    if (peep->VoucherType == VOUCHER_TYPE_RIDE_FREE)
                        {
                            peep->VoucherRideId = static_cast<ride_id_t>(value);
                        }
                    else
                        {
                            peep->VoucherShopItem = static_cast<ShopItem>(value);
                        }
                }
        }

    int32_t ScGuest::previousRide_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? EnumValue(peep->PreviousRide) : EnumValue(RIDE_ID_NULL);
    }
 
    int32_t ScGuest::currentRide_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? EnumValue(peep->CurrentRide) : EnumValue(RIDE_ID_NULL);
    }

    uint8_t ScGuest::currentRideStation_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? static_cast<uint8_t>(peep->CurrentRideStation) : 0;
    }

    int32_t ScGuest::interactionRide_get() const
    {
        auto peep = GetGuest();
        return peep != nullptr ? EnumValue(peep->InteractionRideIndex) : EnumValue(RIDE_ID_NULL);
    }

    uint8_t ScGuest::peepState_get() const
    {   
        auto peep = GetGuest();
        return peep != nullptr ? EnumValue(peep->State) : 0;
    }


} // namespace OpenRCT2::Scripting

#endif
