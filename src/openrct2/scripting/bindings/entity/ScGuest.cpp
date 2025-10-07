/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING_REFACTOR

    #include "ScGuest.hpp"

    #include "../../../GameState.h"
    #include "../../../entity/Guest.h"
    #include "../../../localisation/Formatter.h"
    #include "../../../localisation/Formatting.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../object/PeepAnimationsObject.h"
    #include "../../../peep/PeepAnimations.h"
    #include "../../../ride/RideEntry.h"

namespace OpenRCT2::Scripting
{
    static const EnumMap<PeepThoughtType> ThoughtTypeMap(
        {
            { "cant_afford_ride", PeepThoughtType::CantAffordRide },
            { "spent_money", PeepThoughtType::SpentMoney },
            { "sick", PeepThoughtType::Sick },
            { "very_sick", PeepThoughtType::VerySick },
            { "more_thrilling", PeepThoughtType::MoreThrilling },
            { "intense", PeepThoughtType::Intense },
            { "havent_finished", PeepThoughtType::HaventFinished },
            { "sickening", PeepThoughtType::Sickening },
            { "bad_value", PeepThoughtType::BadValue },
            { "go_home", PeepThoughtType::GoHome },
            { "good_value", PeepThoughtType::GoodValue },
            { "already_got", PeepThoughtType::AlreadyGot },
            { "cant_afford_item", PeepThoughtType::CantAffordItem },
            { "not_hungry", PeepThoughtType::NotHungry },
            { "not_thirsty", PeepThoughtType::NotThirsty },
            { "drowning", PeepThoughtType::Drowning },
            { "lost", PeepThoughtType::Lost },
            { "was_great", PeepThoughtType::WasGreat },
            { "queuing_ages", PeepThoughtType::QueuingAges },
            { "tired", PeepThoughtType::Tired },
            { "hungry", PeepThoughtType::Hungry },
            { "thirsty", PeepThoughtType::Thirsty },
            { "toilet", PeepThoughtType::Toilet },
            { "cant_find", PeepThoughtType::CantFind },
            { "not_paying", PeepThoughtType::NotPaying },
            { "not_while_raining", PeepThoughtType::NotWhileRaining },
            { "bad_litter", PeepThoughtType::BadLitter },
            { "cant_find_exit", PeepThoughtType::CantFindExit },
            { "get_off", PeepThoughtType::GetOff },
            { "get_out", PeepThoughtType::GetOut },
            { "not_safe", PeepThoughtType::NotSafe },
            { "path_disgusting", PeepThoughtType::PathDisgusting },
            { "crowded", PeepThoughtType::Crowded },
            { "vandalism", PeepThoughtType::Vandalism },
            { "scenery", PeepThoughtType::Scenery },
            { "very_clean", PeepThoughtType::VeryClean },
            { "fountains", PeepThoughtType::Fountains },
            { "music", PeepThoughtType::Music },
            { "balloon", PeepThoughtType::Balloon },
            { "toy", PeepThoughtType::Toy },
            { "map", PeepThoughtType::Map },
            { "photo", PeepThoughtType::Photo },
            { "umbrella", PeepThoughtType::Umbrella },
            { "drink", PeepThoughtType::Drink },
            { "burger", PeepThoughtType::Burger },
            { "chips", PeepThoughtType::Chips },
            { "ice_cream", PeepThoughtType::IceCream },
            { "candyfloss", PeepThoughtType::Candyfloss },
            { "pizza", PeepThoughtType::Pizza },
            { "popcorn", PeepThoughtType::Popcorn },
            { "hot_dog", PeepThoughtType::HotDog },
            { "tentacle", PeepThoughtType::Tentacle },
            { "hat", PeepThoughtType::Hat },
            { "toffee_apple", PeepThoughtType::ToffeeApple },
            { "tshirt", PeepThoughtType::Tshirt },
            { "doughnut", PeepThoughtType::Doughnut },
            { "coffee", PeepThoughtType::Coffee },
            { "chicken", PeepThoughtType::Chicken },
            { "lemonade", PeepThoughtType::Lemonade },
            { "wow", PeepThoughtType::Wow },
            { "wow2", PeepThoughtType::Wow2 },
            { "watched", PeepThoughtType::Watched },
            { "balloon_much", PeepThoughtType::BalloonMuch },
            { "toy_much", PeepThoughtType::ToyMuch },
            { "map_much", PeepThoughtType::MapMuch },
            { "photo_much", PeepThoughtType::PhotoMuch },
            { "umbrella_much", PeepThoughtType::UmbrellaMuch },
            { "drink_much", PeepThoughtType::DrinkMuch },
            { "burger_much", PeepThoughtType::BurgerMuch },
            { "chips_much", PeepThoughtType::ChipsMuch },
            { "ice_cream_much", PeepThoughtType::IceCreamMuch },
            { "candyfloss_much", PeepThoughtType::CandyflossMuch },
            { "pizza_much", PeepThoughtType::PizzaMuch },
            { "popcorn_much", PeepThoughtType::PopcornMuch },
            { "hot_dog_much", PeepThoughtType::HotDogMuch },
            { "tentacle_much", PeepThoughtType::TentacleMuch },
            { "hat_much", PeepThoughtType::HatMuch },
            { "toffee_apple_much", PeepThoughtType::ToffeeAppleMuch },
            { "tshirt_much", PeepThoughtType::TshirtMuch },
            { "doughnut_much", PeepThoughtType::DoughnutMuch },
            { "coffee_much", PeepThoughtType::CoffeeMuch },
            { "chicken_much", PeepThoughtType::ChickenMuch },
            { "lemonade_much", PeepThoughtType::LemonadeMuch },
            { "photo2", PeepThoughtType::Photo2 },
            { "photo3", PeepThoughtType::Photo3 },
            { "photo4", PeepThoughtType::Photo4 },
            { "pretzel", PeepThoughtType::Pretzel },
            { "hot_chocolate", PeepThoughtType::HotChocolate },
            { "iced_tea", PeepThoughtType::IcedTea },
            { "funnel_cake", PeepThoughtType::FunnelCake },
            { "sunglasses", PeepThoughtType::Sunglasses },
            { "beef_noodles", PeepThoughtType::BeefNoodles },
            { "fried_rice_noodles", PeepThoughtType::FriedRiceNoodles },
            { "wonton_soup", PeepThoughtType::WontonSoup },
            { "meatball_soup", PeepThoughtType::MeatballSoup },
            { "fruit_juice", PeepThoughtType::FruitJuice },
            { "soybean_milk", PeepThoughtType::SoybeanMilk },
            { "sujongkwa", PeepThoughtType::Sujongkwa },
            { "sub_sandwich", PeepThoughtType::SubSandwich },
            { "cookie", PeepThoughtType::Cookie },
            { "roast_sausage", PeepThoughtType::RoastSausage },
            { "photo2_much", PeepThoughtType::Photo2Much },
            { "photo3_much", PeepThoughtType::Photo3Much },
            { "photo4_much", PeepThoughtType::Photo4Much },
            { "pretzel_much", PeepThoughtType::PretzelMuch },
            { "hot_chocolate_much", PeepThoughtType::HotChocolateMuch },
            { "iced_tea_much", PeepThoughtType::IcedTeaMuch },
            { "funnel_cake_much", PeepThoughtType::FunnelCakeMuch },
            { "sunglasses_much", PeepThoughtType::SunglassesMuch },
            { "beef_noodles_much", PeepThoughtType::BeefNoodlesMuch },
            { "fried_rice_noodles_much", PeepThoughtType::FriedRiceNoodlesMuch },
            { "wonton_soup_much", PeepThoughtType::WontonSoupMuch },
            { "meatball_soup_much", PeepThoughtType::MeatballSoupMuch },
            { "fruit_juice_much", PeepThoughtType::FruitJuiceMuch },
            { "soybean_milk_much", PeepThoughtType::SoybeanMilkMuch },
            { "sujongkwa_much", PeepThoughtType::SujongkwaMuch },
            { "sub_sandwich_much", PeepThoughtType::SubSandwichMuch },
            { "cookie_much", PeepThoughtType::CookieMuch },
            { "roast_sausage_much", PeepThoughtType::RoastSausageMuch },
            { "help", PeepThoughtType::Help },
            { "running_out", PeepThoughtType::RunningOut },
            { "new_ride", PeepThoughtType::NewRide },
            { "nice_ride_deprecated", PeepThoughtType::NiceRideDeprecated },
            { "excited_deprecated", PeepThoughtType::ExcitedDeprecated },
            { "here_we_are", PeepThoughtType::HereWeAre },
        });

    JSValue ScGuest::New(JSContext* ctx, EntityId entityId)
    {
        JSValue obj = ScPeep::New(ctx, entityId);
        AddFuncs(ctx, obj);
        return obj;
    }

    void ScGuest::AddFuncs(JSContext* ctx, JSValue obj)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("tshirtColour", &ScGuest::tshirtColour_get, &ScGuest::tshirtColour_set),
            JS_CGETSET_DEF("trousersColour", &ScGuest::trousersColour_get, &ScGuest::trousersColour_set),
            JS_CGETSET_DEF("balloonColour", &ScGuest::balloonColour_get, &ScGuest::balloonColour_set),
            JS_CGETSET_DEF("hatColour", &ScGuest::hatColour_get, &ScGuest::hatColour_set),
            JS_CGETSET_DEF("umbrellaColour", &ScGuest::umbrellaColour_get, &ScGuest::umbrellaColour_set),
            JS_CGETSET_DEF("happiness", &ScGuest::happiness_get, &ScGuest::happiness_set),
            JS_CGETSET_DEF("happinessTarget", &ScGuest::happinessTarget_get, &ScGuest::happinessTarget_set),
            JS_CGETSET_DEF("nausea", &ScGuest::nausea_get, &ScGuest::nausea_set),
            JS_CGETSET_DEF("nauseaTarget", &ScGuest::nauseaTarget_get, &ScGuest::nauseaTarget_set),
            JS_CGETSET_DEF("hunger", &ScGuest::hunger_get, &ScGuest::hunger_set),
            JS_CGETSET_DEF("thirst", &ScGuest::thirst_get, &ScGuest::thirst_set),
            JS_CGETSET_DEF("toilet", &ScGuest::toilet_get, &ScGuest::toilet_set),
            JS_CGETSET_DEF("mass", &ScGuest::mass_get, &ScGuest::mass_set),
            JS_CGETSET_DEF("minIntensity", &ScGuest::minIntensity_get, &ScGuest::minIntensity_set),
            JS_CGETSET_DEF("maxIntensity", &ScGuest::maxIntensity_get, &ScGuest::maxIntensity_set),
            JS_CGETSET_DEF("nauseaTolerance", &ScGuest::nauseaTolerance_get, &ScGuest::nauseaTolerance_set),
            JS_CGETSET_DEF("cash", &ScGuest::cash_get, &ScGuest::cash_set),
            JS_CGETSET_DEF("isInPark", &ScGuest::isInPark_get, nullptr),
            JS_CGETSET_DEF("isLost", &ScGuest::isLost_get, nullptr),
            JS_CGETSET_DEF("lostCountdown", &ScGuest::lostCountdown_get, &ScGuest::lostCountdown_set),
            JS_CGETSET_DEF("favouriteRide", &ScGuest::favouriteRide_get, &ScGuest::favouriteRide_set),
            JS_CGETSET_DEF("thoughts", &ScGuest::thoughts_get, nullptr),
            JS_CGETSET_DEF("items", &ScGuest::items_get, nullptr),
            JS_CGETSET_DEF("availableAnimations", &ScGuest::availableAnimations_get, nullptr),
            JS_CGETSET_DEF("animation", &ScGuest::animation_get, &ScGuest::animation_set),
            JS_CGETSET_DEF("animationOffset", &ScGuest::animationOffset_get, &ScGuest::animationOffset_set),
            JS_CGETSET_DEF("animationLength", &ScGuest::animationLength_get, nullptr),
            JS_CFUNC_DEF("getAnimationSpriteIds", 2, &ScGuest::getAnimationSpriteIds),
            JS_CFUNC_DEF("hasItem", 1, &ScGuest::has_item),
            JS_CFUNC_DEF("giveItem", 1, &ScGuest::give_item),
            JS_CFUNC_DEF("removeItem", 1, &ScGuest::remove_item),
            JS_CFUNC_DEF("removeAllItems", 0, &ScGuest::remove_all_items),
        };
        JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
    }

    Guest* ScGuest::GetGuest(JSValue thisVal)
    {
        auto id = GetEntityId(thisVal);
        return getGameState().entities.GetEntity<Guest>(id);
    }

    JSValue ScGuest::tshirtColour_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->TshirtColour : 0);
    }
    JSValue ScGuest::tshirtColour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->TshirtColour = value;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::trousersColour_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->TrousersColour : 0);
    }
    JSValue ScGuest::trousersColour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->TrousersColour = value;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::balloonColour_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->BalloonColour : 0);
    }
    JSValue ScGuest::balloonColour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->BalloonColour = value;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::hatColour_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->HatColour : 0);
    }
    JSValue ScGuest::hatColour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->HatColour = value;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::umbrellaColour_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->UmbrellaColour : 0);
    }
    JSValue ScGuest::umbrellaColour_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->UmbrellaColour = value;
            peep->Invalidate();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::happiness_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Happiness : 0);
    }
    JSValue ScGuest::happiness_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Happiness = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::happinessTarget_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->HappinessTarget : 0);
    }
    JSValue ScGuest::happinessTarget_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->HappinessTarget = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::nausea_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Nausea : 0);
    }
    JSValue ScGuest::nausea_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Nausea = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::nauseaTarget_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->NauseaTarget : 0);
    }
    JSValue ScGuest::nauseaTarget_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->NauseaTarget = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::hunger_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Hunger : 0);
    }
    JSValue ScGuest::hunger_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Hunger = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::thirst_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Thirst : 0);
    }
    JSValue ScGuest::thirst_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Thirst = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::toilet_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Toilet : 0);
    }
    JSValue ScGuest::toilet_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Toilet = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::mass_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Mass : 0);
    }
    JSValue ScGuest::mass_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Mass = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::minIntensity_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Intensity.GetMinimum() : 0);
    }
    JSValue ScGuest::minIntensity_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Intensity = peep->Intensity.WithMinimum(value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::maxIntensity_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->Intensity.GetMaximum() : 0);
    }
    JSValue ScGuest::maxIntensity_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->Intensity = peep->Intensity.WithMaximum(value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::nauseaTolerance_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? EnumValue(peep->NauseaTolerance) : 0);
    }
    JSValue ScGuest::nauseaTolerance_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->NauseaTolerance = static_cast<PeepNauseaTolerance>(std::min<uint8_t>(value, 3));
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::cash_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewInt32(ctx, peep != nullptr ? peep->CashInPocket : 0);
    }
    JSValue ScGuest::cash_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_INT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->CashInPocket = std::max(0, value);
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::isInPark_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewBool(ctx, peep != nullptr && !peep->OutsideOfPark);
    }

    JSValue ScGuest::isLost_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewBool(ctx, peep != nullptr && peep->GuestIsLostCountdown < 90);
    }

    JSValue ScGuest::lostCountdown_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        return JS_NewUint32(ctx, peep != nullptr ? peep->GuestIsLostCountdown : 0);
    }
    JSValue ScGuest::lostCountdown_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(value, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->GuestIsLostCountdown = value;
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::favouriteRide_get(JSContext* ctx, JSValue thisVal)
    {
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            if (peep->FavouriteRide != RideId::GetNull())
            {
                return JS_NewUint32(ctx, peep->FavouriteRide.ToUnderlying());
            }
        }
        return JS_NULL;
    }

    JSValue ScGuest::favouriteRide_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            auto& gameState = getGameState();
            if (JS_IsNull(jsValue))
            {
                peep->FavouriteRide = RideId::GetNull();
            }
            else if (JS_IsNumber(jsValue))
            {
                JS_UNPACK_UINT32(rideId, ctx, jsValue);
                if (rideId < gameState.rides.size() && gameState.rides[rideId].type != kRideTypeNull)
                {
                    peep->FavouriteRide = RideId::FromUnderlying(rideId);
                }
            }
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::thoughts_get(JSContext* ctx, JSValue thisVal)
    {
        auto array = JS_NewArray(ctx);

        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            auto index = 0;
            for (const auto& thought : peep->Thoughts)
            {
                if (thought.type == PeepThoughtType::None)
                    break;
                if (thought.freshness == 0)
                    continue;
                auto scThought = gScThought.New(ctx, thought);
                JS_SetPropertyInt64(ctx, array, index++, scThought);
            }
        }

        return array;
    }

    JSValue ScGuest::items_get(JSContext* ctx, JSValue thisVal)
    {
        auto array = JS_NewArray(ctx);

        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            auto index = 0;
            for (const auto& itemEnumPair : ShopItemMap)
            {
                auto shopItem = itemEnumPair.second;
                if (!peep->HasItem(shopItem))
                {
                    continue;
                }

                // GuestItem
                auto obj = JS_NewObject(ctx);
                JS_SetPropertyStr(ctx, obj, "type", JSFromStdString(ctx, itemEnumPair.first));

                if (shopItem == ShopItem::voucher)
                {
                    // Voucher
                    JS_SetPropertyStr(ctx, obj, "voucherType", JSFromStdString(ctx, VoucherTypeMap[peep->VoucherType]));
                    if (peep->VoucherType == VOUCHER_TYPE_RIDE_FREE)
                    {
                        // RideVoucher
                        JS_SetPropertyStr(ctx, obj, "rideId", JS_NewUint32(ctx, peep->VoucherRideId.ToUnderlying()));
                    }
                    else if (peep->VoucherType == VOUCHER_TYPE_FOOD_OR_DRINK_FREE)
                    {
                        // FoodDrinkVoucher
                        JS_SetPropertyStr(ctx, obj, "item", JSFromStdString(ctx, ShopItemMap[peep->VoucherShopItem]));
                    }
                }
                else if (GetShopItemDescriptor(shopItem).IsPhoto())
                {
                    // GuestPhoto
                    RideId rideId;
                    switch (shopItem)
                    {
                        case ShopItem::photo:
                            rideId = peep->Photo1RideRef;
                            break;
                        case ShopItem::photo2:
                            rideId = peep->Photo2RideRef;
                            break;
                        case ShopItem::photo3:
                            rideId = peep->Photo3RideRef;
                            break;
                        case ShopItem::photo4:
                            rideId = peep->Photo4RideRef;
                            break;
                        default:
                            // This should not be possible
                            JS_ThrowPlainError(ctx, "Item is photo without a ride ref.");
                            return JS_EXCEPTION;
                    }

                    JS_SetPropertyStr(ctx, obj, "rideId", JS_NewUint32(ctx, rideId.ToUnderlying()));
                }

                JS_SetPropertyInt64(ctx, array, index++, obj);
            }
        }

        return array;
    }

    bool ScGuest::has_item(JSContext* ctx, JSValue thisVal, JSValue item)
    {
        auto peep = GetGuest(thisVal);
        if (peep == nullptr)
        {
            return false;
        }

        auto type = JSToOptionalStdString(ctx, item, "type");
        if (!type.has_value())
        {
            return false;
        }

        // GuestItem
        auto shopItem = ShopItemMap.TryGet(type.value());
        if (!shopItem || !peep->HasItem(*shopItem))
        {
            return false;
        }

        if (*shopItem == ShopItem::voucher)
        {
            auto voucherType = JSToOptionalStdString(ctx, item, "voucherType");
            if (voucherType.has_value())
            {
                // Voucher
                auto voucher = VoucherTypeMap.TryGet(voucherType.value());
                if (!voucher || *voucher != peep->VoucherType)
                {
                    return false;
                }

                if (*voucher == VOUCHER_TYPE_RIDE_FREE)
                {
                    auto rideId = JSToOptionalUint(ctx, item, "rideId");
                    if (rideId.has_value())
                    {
                        // RideVoucher
                        if (rideId.value() != peep->VoucherRideId.ToUnderlying())
                        {
                            return false;
                        }
                    }
                }
                else if (*voucher == VOUCHER_TYPE_FOOD_OR_DRINK_FREE)
                {
                    auto foodItem = JSToOptionalStdString(ctx, item, "item");
                    if (foodItem.has_value())
                    {
                        // FoodDrinkVoucher
                        auto voucherItem = ShopItemMap.TryGet(foodItem.value());
                        if (!voucherItem || *voucherItem != peep->VoucherShopItem)
                        {
                            return false;
                        }
                    }
                }
            }
        }
        else if (GetShopItemDescriptor(*shopItem).IsPhoto())
        {
            // GuestPhoto
            auto rideId = JSToOptionalUint(ctx, item, "rideId");
            if (rideId.has_value())
            {
                switch (*shopItem)
                {
                    case ShopItem::photo:
                        if (rideId.value() != peep->Photo1RideRef.ToUnderlying())
                        {
                            return false;
                        }
                        break;
                    case ShopItem::photo2:
                        if (rideId.value() != peep->Photo2RideRef.ToUnderlying())
                        {
                            return false;
                        }
                        break;
                    case ShopItem::photo3:
                        if (rideId.value() != peep->Photo3RideRef.ToUnderlying())
                        {
                            return false;
                        }
                        break;
                    case ShopItem::photo4:
                        if (rideId.value() != peep->Photo4RideRef.ToUnderlying())
                        {
                            return false;
                        }
                        break;
                    default:
                        return false;
                }
            }
        }
        return true;
    }

    JSValue ScGuest::has_item(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(item, ctx, argv[0]);
        auto result = has_item(ctx, thisVal, item);
        return JS_NewBool(ctx, result);
    }

    JSValue ScGuest::give_item(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(item, ctx, argv[0]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto peep = GetGuest(thisVal);
        if (peep == nullptr)
        {
            return JS_UNDEFINED;
        }

        // GuestItem
        auto type = JSToOptionalStdString(ctx, item, "type");
        if (!type.has_value())
        {
            JS_ThrowPlainError(ctx, "Invalid 'type'.");
            return JS_EXCEPTION;
        }

        auto shopItem = ShopItemMap.TryGet(type.value());
        if (!shopItem)
        {
            JS_ThrowPlainError(ctx, "Invalid 'type'.");
            return JS_EXCEPTION;
        }

        if (*shopItem == ShopItem::voucher)
        {
            // Voucher
            auto voucherTypeName = JSToOptionalStdString(ctx, item, "voucherType");
            if (!voucherTypeName.has_value())
            {
                JS_ThrowPlainError(ctx, "Invalid 'voucherType'.");
                return JS_EXCEPTION;
            }

            auto voucherType = VoucherTypeMap.TryGet(voucherTypeName.value());
            if (!voucherType)
            {
                JS_ThrowPlainError(ctx, "Invalid 'voucherType'.");
                return JS_EXCEPTION;
            }

            if (*voucherType == VOUCHER_TYPE_RIDE_FREE)
            {
                // RideVoucher
                auto rideId = JSToOptionalUint(ctx, item, "rideId");
                if (!rideId.has_value())
                {
                    JS_ThrowPlainError(ctx, "Invalid 'rideId'.");
                    return JS_EXCEPTION;
                }

                peep->VoucherRideId = RideId::FromUnderlying(rideId.value());
            }
            else if (*voucherType == VOUCHER_TYPE_FOOD_OR_DRINK_FREE)
            {
                // FoodDrinkVoucher
                auto itemName = JSToOptionalStdString(ctx, item, "item");
                if (!itemName.has_value())
                {
                    JS_ThrowPlainError(ctx, "Invalid 'item' (for food/drink voucher).");
                    return JS_EXCEPTION;
                }

                auto voucherItem = ShopItemMap.TryGet(itemName.value());
                if (!voucherItem)
                {
                    JS_ThrowPlainError(ctx, "Invalid 'item' (for food/drink voucher).");
                    return JS_EXCEPTION;
                }

                peep->VoucherShopItem = *voucherItem;
            }

            peep->VoucherType = *voucherType;
        }
        else if (GetShopItemDescriptor(*shopItem).IsPhoto())
        {
            // GuestPhoto
            auto rideId = JSToOptionalUint(ctx, item, "rideId");
            if (!rideId.has_value())
            {
                JS_ThrowPlainError(ctx, "Invalid 'rideId'.");
                return JS_EXCEPTION;
            }

            switch (*shopItem)
            {
                case ShopItem::photo:
                    peep->Photo1RideRef = RideId::FromUnderlying(rideId.value());
                    break;
                case ShopItem::photo2:
                    peep->Photo2RideRef = RideId::FromUnderlying(rideId.value());
                    break;
                case ShopItem::photo3:
                    peep->Photo3RideRef = RideId::FromUnderlying(rideId.value());
                    break;
                case ShopItem::photo4:
                    peep->Photo4RideRef = RideId::FromUnderlying(rideId.value());
                    break;
                default:
                    return JS_UNDEFINED;
            }
        }

        peep->GiveItem(*shopItem);
        peep->UpdateAnimationGroup();
        return JS_UNDEFINED;
    }

    JSValue ScGuest::remove_item(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_OBJECT(item, ctx, argv[0]);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        if (has_item(ctx, thisVal, item))
        {
            auto type = JSToStdString(ctx, item, "type");

            // Since guests can only have one item of a type and this item matches, remove it.
            auto peep = GetGuest(thisVal);
            peep->RemoveItem(ShopItemMap[type]);
            peep->UpdateAnimationGroup();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::remove_all_items(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();
        auto peep = GetGuest(thisVal);
        if (peep != nullptr)
        {
            peep->RemoveAllItems();
            peep->UpdateAnimationGroup();
        }
        return JS_UNDEFINED;
    }

    JSValue ScGuest::availableAnimations_get(JSContext* ctx, JSValue thisVal)
    {
        auto availableAnimations = JS_NewArray(ctx);
        auto index = 0;
        for (auto& animation : getAnimationsByPeepType(AnimationPeepType::guest))
        {
            JS_SetPropertyInt64(ctx, availableAnimations, index++, JSFromStdString(ctx, animation.first));
        }
        return availableAnimations;
    }

    JSValue ScGuest::getAnimationSpriteIds(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        JS_UNPACK_STR(groupKey, ctx, argv[0]);
        JS_UNPACK_UINT32(rotation, ctx, argv[1]);
        JSValue spriteIds = JS_NewArray(ctx);

        auto& availableGuestAnimations = getAnimationsByPeepType(AnimationPeepType::guest);
        auto animationType = availableGuestAnimations.TryGet(groupKey);
        if (animationType == std::nullopt)
        {
            return spriteIds;
        }

        auto peep = GetPeep(thisVal);
        if (peep != nullptr)
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

            const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, *animationType);
            auto idx = 0;
            for (auto frameOffset : animationGroup.frameOffsets)
            {
                auto imageId = animationGroup.baseImage;
                if (animationType != PeepAnimationType::hanging)
                    imageId += rotation + frameOffset * 4;
                else
                    imageId += frameOffset;

                JS_SetPropertyInt64(ctx, spriteIds, idx++, JS_NewUint32(ctx, imageId));
            }
        }
        return spriteIds;
    }

    JSValue ScGuest::animation_get(JSContext* ctx, JSValue thisVal)
    {
        auto* peep = GetGuest(thisVal);
        if (peep == nullptr)
        {
            return JS_NULL;
        }

        auto& availableGuestAnimations = getAnimationsByPeepType(AnimationPeepType::guest);
        std::string_view action = availableGuestAnimations[peep->AnimationType];

        // Special consideration for sitting peeps
        // TODO: something funky going on in the state machine
        if (peep->AnimationType == PeepAnimationType::walking && peep->State == PeepState::sitting)
            action = availableGuestAnimations[PeepAnimationType::sittingIdle];

        return JSFromStdString(ctx, action);
    }

    JSValue ScGuest::animation_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_STR(groupKey, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto& availableGuestAnimations = getAnimationsByPeepType(AnimationPeepType::guest);
        auto newType = availableGuestAnimations.TryGet(groupKey);
        if (newType == std::nullopt)
        {
            JS_ThrowPlainError(ctx, "Invalid animation for this guest (%s)", groupKey.data());
            return JS_EXCEPTION;
        }

        auto* peep = GetGuest(thisVal);
        peep->AnimationType = peep->NextAnimationType = *newType;

        auto offset = 0;
        if (peep->IsActionWalking())
            peep->WalkingAnimationFrameNum = offset;
        else
            peep->AnimationFrameNum = offset;

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        peep->AnimationImageIdOffset = animationGroup.frameOffsets[offset];
        peep->Invalidate();
        peep->UpdateSpriteBoundingBox();
        peep->Invalidate();
        return JS_UNDEFINED;
    }

    JSValue ScGuest::animationOffset_get(JSContext* ctx, JSValue thisVal)
    {
        auto* peep = GetGuest(thisVal);
        if (peep == nullptr)
        {
            return JS_NewUint32(ctx, 0);
        }

        auto frame = peep->IsActionWalking() ? peep->WalkingAnimationFrameNum : peep->AnimationFrameNum;
        return JS_NewUint32(ctx, frame);
    }

    JSValue ScGuest::animationOffset_set(JSContext* ctx, JSValue thisVal, JSValue jsValue)
    {
        JS_UNPACK_UINT32(offset, ctx, jsValue);
        JS_THROW_IF_GAME_STATE_NOT_MUTABLE();

        auto* peep = GetGuest(thisVal);

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        auto length = animationGroup.frameOffsets.size();
        offset %= length;

        if (peep->IsActionWalking())
            peep->WalkingAnimationFrameNum = offset;
        else
            peep->AnimationFrameNum = offset;

        peep->AnimationImageIdOffset = animationGroup.frameOffsets[offset];
        peep->UpdateSpriteBoundingBox();
        return JS_UNDEFINED;
    }

    JSValue ScGuest::animationLength_get(JSContext* ctx, JSValue thisVal)
    {
        auto* peep = GetGuest(thisVal);
        if (peep == nullptr)
        {
            return JS_NewUint32(ctx, 0);
        }

        auto& objManager = GetContext()->GetObjectManager();
        auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

        const auto& animationGroup = animObj->GetPeepAnimation(peep->AnimationGroup, peep->AnimationType);
        return JS_NewUint32(ctx, static_cast<uint32_t>(animationGroup.frameOffsets.size()));
    }

    using OpaqueThoughtData = struct
    {
        PeepThought thought;
    };

    JSValue ScThought::New(JSContext* ctx, PeepThought thought)
    {
        static constexpr JSCFunctionListEntry funcs[] = {
            JS_CGETSET_DEF("type", &ScThought::type_get, nullptr),
            JS_CGETSET_DEF("item", &ScThought::item_get, nullptr),
            JS_CGETSET_DEF("freshness", &ScThought::freshness_get, nullptr),
            JS_CGETSET_DEF("freshTimeout", &ScThought::freshTimeout_get, nullptr),
            JS_CFUNC_DEF("toString", 0, &ScThought::toString),
        };
        return MakeWithOpaque(ctx, funcs, new OpaqueThoughtData{ thought });
    }

    void ScThought::Register(JSContext* ctx)
    {
        RegisterBaseStr(ctx, "Thought", Finalize);
    }

    void ScThought::Finalize(JSRuntime* rt, JSValue thisVal)
    {
        OpaqueThoughtData* data = gScThought.GetOpaque<OpaqueThoughtData*>(thisVal);
        if (data)
            delete data;
    }

    PeepThought ScThought::GetThought(JSValue thisVal)
    {
        OpaqueThoughtData* data = gScThought.GetOpaque<OpaqueThoughtData*>(thisVal);
        return data->thought;
    }

    JSValue ScThought::type_get(JSContext* ctx, JSValue thisVal)
    {
        auto thought = GetThought(thisVal);
        return JSFromStdString(ctx, ThoughtTypeMap[thought.type]);
    }

    JSValue ScThought::item_get(JSContext* ctx, JSValue thisVal)
    {
        auto thought = GetThought(thisVal);
        return JS_NewUint32(ctx, thought.item);
    }

    JSValue ScThought::freshness_get(JSContext* ctx, JSValue thisVal)
    {
        auto thought = GetThought(thisVal);
        return JS_NewUint32(ctx, thought.freshness);
    }

    JSValue ScThought::freshTimeout_get(JSContext* ctx, JSValue thisVal)
    {
        auto thought = GetThought(thisVal);
        return JS_NewUint32(ctx, thought.fresh_timeout);
    }

    JSValue ScThought::toString(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
    {
        auto thought = GetThought(thisVal);
        // format string with arguments
        auto ft = Formatter();
        PeepThoughtSetFormatArgs(&thought, ft);
        auto result = FormatStringIDLegacy(STR_STRINGID, ft.Data());
        return JSFromStdString(ctx, result);
    }

} // namespace OpenRCT2::Scripting

#endif
