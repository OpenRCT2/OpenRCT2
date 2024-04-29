/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../entity/Guest.h"
#    include "../ride/ScRide.hpp"
#    include "ScPeep.hpp"

namespace OpenRCT2::Scripting
{
    static const DukEnumMap<ShopItem> ShopItemMap({
        { "beef_noodles", ShopItem::BeefNoodles },
        { "burger", ShopItem::Burger },
        { "candyfloss", ShopItem::Candyfloss },
        { "chicken", ShopItem::Chicken },
        { "chips", ShopItem::Chips },
        { "chocolate", ShopItem::Chocolate },
        { "cookie", ShopItem::Cookie },
        { "doughnut", ShopItem::Doughnut },
        { "hot_dog", ShopItem::HotDog },
        { "fried_rice_noodles", ShopItem::FriedRiceNoodles },
        { "funnel_cake", ShopItem::FunnelCake },
        { "ice_cream", ShopItem::IceCream },
        { "meatball_soup", ShopItem::MeatballSoup },
        { "pizza", ShopItem::Pizza },
        { "popcorn", ShopItem::Popcorn },
        { "pretzel", ShopItem::Pretzel },
        { "roast_sausage", ShopItem::RoastSausage },
        { "sub_sandwich", ShopItem::SubSandwich },
        { "tentacle", ShopItem::Tentacle },
        { "toffee_apple", ShopItem::ToffeeApple },
        { "wonton_soup", ShopItem::WontonSoup },
        { "coffee", ShopItem::Coffee },
        { "drink", ShopItem::Drink },
        { "fruit_juice", ShopItem::FruitJuice },
        { "iced_tea", ShopItem::IcedTea },
        { "lemonade", ShopItem::Lemonade },
        { "soybean_milk", ShopItem::SoybeanMilk },
        { "sujeonggwa", ShopItem::Sujeonggwa },
        { "balloon", ShopItem::Balloon },
        { "hat", ShopItem::Hat },
        { "map", ShopItem::Map },
        { "sunglasses", ShopItem::Sunglasses },
        { "toy", ShopItem::Toy },
        { "tshirt", ShopItem::TShirt },
        { "umbrella", ShopItem::Umbrella },
        { "photo1", ShopItem::Photo },
        { "photo2", ShopItem::Photo2 },
        { "photo3", ShopItem::Photo3 },
        { "photo4", ShopItem::Photo4 },
        { "voucher", ShopItem::Voucher },
        { "empty_bottle", ShopItem::EmptyBottle },
        { "empty_bowl_blue", ShopItem::EmptyBowlBlue },
        { "empty_bowl_red", ShopItem::EmptyBowlRed },
        { "empty_box", ShopItem::EmptyBox },
        { "empty_burger_box", ShopItem::EmptyBurgerBox },
        { "empty_can", ShopItem::EmptyCan },
        { "empty_cup", ShopItem::EmptyCup },
        { "empty_drink_carton", ShopItem::EmptyDrinkCarton },
        { "empty_juice_cup", ShopItem::EmptyJuiceCup },
        { "rubbish", ShopItem::Rubbish },
    });
    // Since the ShopItem enum is missing values and includes ShopItem::Admission (something a
    // guest cannot carry), 6 is subtracted from the value.
    static_assert((EnumValue(ShopItem::Count) - 6) == 50, "ShopItem::Count changed, update scripting binding!");

    static const DukEnumMap<uint32_t> VoucherTypeMap({
        { "entry_free", VOUCHER_TYPE_PARK_ENTRY_FREE },
        { "entry_half_price", VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE },
        { "ride_free", VOUCHER_TYPE_RIDE_FREE },
        { "food_drink_free", VOUCHER_TYPE_FOOD_OR_DRINK_FREE },
    });

    class ScThought
    {
    private:
        PeepThought _backing;

    public:
        ScThought(PeepThought backing);

        static void Register(duk_context* ctx);

    private:
        std::string type_get() const;
        uint16_t item_get() const;
        uint8_t freshness_get() const;
        uint8_t freshTimeout_get() const;
        std::string toString() const;
    };

    class ScGuest : public ScPeep
    {
    public:
        ScGuest(EntityId id);

        static void Register(duk_context* ctx);

    private:
        Guest* GetGuest() const;

        uint8_t tshirtColour_get() const;
        void tshirtColour_set(uint8_t value);

        uint8_t trousersColour_get() const;
        void trousersColour_set(uint8_t value);

        uint8_t balloonColour_get() const;
        void balloonColour_set(uint8_t value);

        uint8_t hatColour_get() const;
        void hatColour_set(uint8_t value);

        uint8_t umbrellaColour_get() const;
        void umbrellaColour_set(uint8_t value);

        uint8_t happiness_get() const;
        void happiness_set(uint8_t value);

        uint8_t happinessTarget_get() const;
        void happinessTarget_set(uint8_t value);

        uint8_t nausea_get() const;
        void nausea_set(uint8_t value);

        uint8_t nauseaTarget_get() const;
        void nauseaTarget_set(uint8_t value);

        uint8_t hunger_get() const;
        void hunger_set(uint8_t value);

        uint8_t thirst_get() const;
        void thirst_set(uint8_t value);

        uint8_t toilet_get() const;
        void toilet_set(uint8_t value);

        uint8_t mass_get() const;
        void mass_set(uint8_t value);

        uint8_t minIntensity_get() const;
        void minIntensity_set(uint8_t value);

        uint8_t maxIntensity_get() const;
        void maxIntensity_set(uint8_t value);

        uint8_t nauseaTolerance_get() const;
        void nauseaTolerance_set(uint8_t value);

        int32_t cash_get() const;
        void cash_set(int32_t value);

        bool isInPark_get() const;

        bool isLost_get() const;

        uint8_t lostCountdown_get() const;
        void lostCountdown_set(uint8_t value);

        std::shared_ptr<ScRide> rideHeadedTo_get() const;
        void rideHeadedTo_set(int32_t rideId);

        DukValue thoughts_get() const;

        DukValue items_get() const;
        bool has_item(const DukValue& item) const;
        void give_item(const DukValue& item) const;
        void remove_item(const DukValue& item) const;
        void remove_all_items() const;
    };

} // namespace OpenRCT2::Scripting

#endif
